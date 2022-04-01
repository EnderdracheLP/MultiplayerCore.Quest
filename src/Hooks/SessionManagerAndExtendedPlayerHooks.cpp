#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "Hooks/SessionManagerAndExtendedPlayerHooks.hpp"
#include "GlobalFields.hpp"
#include "Hooks/EnvironmentAndAvatarHooks.hpp"
#include "Players/MpPlayerData.hpp"
#include "UI/CenterScreenLoading.hpp"
#include "Utils/SemVerChecker.hpp"
#include "Utilities.hpp"

#include "Beatmaps/Packets/MpBeatmapPacket.hpp"
#include "Beatmaps/Abstractions/MpBeatmapLevel.hpp"
#include "Beatmaps/NetworkBeatmapLevel.hpp"

#include "Networking/MpPacketSerializer.hpp"

#include "GlobalNamespace/IPlatformUserModel.hpp"
#include "GlobalNamespace/LocalNetworkPlayerModel.hpp"
#include "UnityEngine/Resources.hpp"
#include "GlobalNamespace/UserInfo.hpp"
#include "GlobalNamespace/PreviewDifficultyBeatmap.hpp"
#include "GlobalNamespace/MultiplayerSessionManager_SessionType.hpp"
#include "GlobalNamespace/LobbyPlayerData.hpp"

#include "CodegenExtensions/ColorUtility.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"
using namespace MultiQuestensions;
using namespace MultiplayerCore;
using namespace GlobalNamespace;

// Handles a HandleMpexBeatmapPacket used to transmit data about a custom song.
static void HandleMpexBeatmapPacket(Beatmaps::Packets::MpBeatmapPacket* packet, GlobalNamespace::IConnectedPlayer* player) {
    getLogger().debug("Player '%s' selected song '%s'", static_cast<std::string>(player->get_userId()).c_str(), static_cast<std::string>(packet->levelHash).c_str());
    BeatmapCharacteristicSO* characteristic = lobbyPlayersDataModel->dyn__beatmapCharacteristicCollection()->GetBeatmapCharacteristicBySerializedName(packet->characteristic);
    Beatmaps::NetworkBeatmapLevel* preview = Beatmaps::NetworkBeatmapLevel::New_ctor(packet);

    IPreviewBeatmapLevel* sendResponse = reinterpret_cast<IPreviewBeatmapLevel*>(preview);
    getLogger().debug("levelId: '%s', songName: '%s', songSubName: '%s', songAuthorName: '%s'", 
        static_cast<std::string>(sendResponse->get_levelID()).c_str(),
        static_cast<std::string>(sendResponse->get_songName()).c_str(),
        static_cast<std::string>(sendResponse->get_songSubName()).c_str(),
        static_cast<std::string>(sendResponse->get_songAuthorName()).c_str()
    );

    lobbyPlayersDataModel->SetPlayerBeatmapLevel(player->get_userId(), GlobalNamespace::PreviewDifficultyBeatmap::New_ctor(reinterpret_cast<IPreviewBeatmapLevel*>(preview), characteristic, packet->difficulty));
}

SafePtr<MultiplayerCore::Players::MpPlayerData> localPlayer;
std::map<std::string, SafePtr<MultiplayerCore::Players::MpPlayerData>> _playerData;
IPlatformUserModel* platformUserModel;

static void HandlePlayerData(MultiplayerCore::Players::MpPlayerData* playerData, IConnectedPlayer* player) {
    const std::string userId = static_cast<std::string>(player->get_userId());
    if (_playerData.contains(userId)) {
        _playerData.at(userId) = playerData;
    }
    else {
        getLogger().info("Received 'MpPlayerData' from '%s' with platformID: '%s' platform: '%d'",
            to_utf8(csstrtostr(player->get_userId())).c_str(),
            to_utf8(csstrtostr(playerData->platformId)).c_str(),
            (int)playerData->platform
        );

        _playerData.emplace(userId, playerData);
        getLogger().debug("MpPlayerData done");
    }
}

void HandlePlayerConnected(IConnectedPlayer* player) {
    try {
        getLogger().debug("HandlePlayerConnected");
        if (player) {
            getLogger().info("Player '%s' joined", static_cast<std::string>(player->get_userId()).c_str());
            getLogger().debug("Sending MpPlayerData");
            if (localPlayer->platformId)
            {
                mpPacketSerializer->Send(localPlayer);
            }
            getLogger().debug("MpPlayerData sent");
        }
    }
    catch (const std::runtime_error& e) {
        getLogger().error("REPORT TO ENDER: %s", e.what());
    }
}

void HandlePlayerDisconnected(IConnectedPlayer* player) {
    getLogger().info("Player '%s' left", static_cast<std::string>(player->get_userId()).c_str());
    _playerData.erase(static_cast<std::string>(player->get_userId()));
}

//void HandleDisconnect(DisconnectedReason* reason) {
//}

MAKE_HOOK_MATCH(SessionManagerStart, &MultiplayerSessionManager::Start, void, MultiplayerSessionManager* self) {

    _multiplayerSessionManager = self;
    SessionManagerStart(_multiplayerSessionManager);
    mpPacketSerializer = Networking::MpPacketSerializer::New_ctor(_multiplayerSessionManager);


    mpPacketSerializer->RegisterCallback<Beatmaps::Packets::MpBeatmapPacket*>(HandleMpexBeatmapPacket);
    mpPacketSerializer->RegisterCallback<Players::MpPlayerData*>(HandlePlayerData);
}

MAKE_HOOK_MATCH(SessionManager_StartSession, &MultiplayerSessionManager::StartSession, void, MultiplayerSessionManager* self, MultiplayerSessionManager_SessionType sessionType, ConnectedPlayerManager* connectedPlayerManager) {
    SessionManager_StartSession(self, sessionType, connectedPlayerManager);
    getLogger().debug("MultiplayerSessionManager.StartSession, creating localPlayer");

        static auto localNetworkPlayerModel = UnityEngine::Resources::FindObjectsOfTypeAll<LocalNetworkPlayerModel*>().get(0);
        static auto UserInfoTask = localNetworkPlayerModel->dyn__platformUserModel()->GetUserInfo();
        static auto action = il2cpp_utils::MakeDelegate<System::Action_1<System::Threading::Tasks::Task*>*>(classof(System::Action_1<System::Threading::Tasks::Task*>*), (std::function<void(System::Threading::Tasks::Task_1<GlobalNamespace::UserInfo*>*)>)[&](System::Threading::Tasks::Task_1<GlobalNamespace::UserInfo*>* userInfoTask) {
            auto userInfo = userInfoTask->get_Result();
            if (userInfo) {
                if (!localPlayer) localPlayer = Players::MpPlayerData::Init(userInfo->dyn_platformUserId(), (Players::Platform)userInfo->dyn_platform().value);
                else {
                    localPlayer->platformId = userInfo->dyn_platformUserId();
                    localPlayer->platform = (Players::Platform)userInfo->dyn_platform().value;
                }
            }
            else getLogger().error("Failed to get local network player!");
            }
        );
        reinterpret_cast<System::Threading::Tasks::Task*>(UserInfoTask)->ContinueWith(action);

        using namespace MultiQuestensions::Utils;
        self->SetLocalPlayerState(getModdedStateStr(), true);
        self->SetLocalPlayerState(getMEStateStr(), MatchesVersion("MappingExtensions", "*"));
        self->SetLocalPlayerState(getNEStateStr(), MatchesVersion("NoodleExtensions", "*"));
        self->SetLocalPlayerState(getChromaStateStr(), MatchesVersion(ChromaID, ChromaVersionRange));

    self->add_playerConnectedEvent(il2cpp_utils::MakeDelegate<System::Action_1<IConnectedPlayer*>*>(classof(System::Action_1<IConnectedPlayer*>*), static_cast<Il2CppObject*>(nullptr), HandlePlayerConnected));
    self->add_playerDisconnectedEvent(il2cpp_utils::MakeDelegate<System::Action_1<IConnectedPlayer*>*>(classof(System::Action_1<IConnectedPlayer*>*), static_cast<Il2CppObject*>(nullptr), HandlePlayerDisconnected));
    //self->add_disconnectedEvent(il2cpp_utils::MakeDelegate<System::Action_1<GlobalNamespace::DisconnectedReason>*>*>(classof(System::Action_1<GlobalNamespace::DisconnectedReason>*>*), static_cast<Il2CppObject*>(nullptr), HandleDisconnect));
}

MAKE_HOOK_MATCH(LobbyPlayersDataModel_HandleMenuRpcManagerGetRecommendedBeatmap, &LobbyPlayersDataModel::HandleMenuRpcManagerGetRecommendedBeatmap, void, LobbyPlayersDataModel* self, StringW userId) {
    getLogger().debug("LobbyPlayersDataModel_HandleMenuRpcManagerGetRecommendedBeatmap Start");
    LobbyPlayerData* localPlayerData = self->dyn__playersData()->get_Item(self->get_localUserId());
    if (localPlayerData->get_beatmapLevel() && localPlayerData->get_beatmapLevel()->get_beatmapLevel()) {
        getLogger().debug("LobbyPlayersDataModel_HandleMenuRpcManagerGetRecommendedBeatmap Get LevelID");
        StringW levelId = localPlayerData->get_beatmapLevel()->get_beatmapLevel()->get_levelID();
        if (Il2CppString::IsNullOrEmpty(levelId))
            return;
        StringW levelHash = Utilities::HashForLevelID(levelId);
        if (Il2CppString::IsNullOrEmpty(levelHash))
            mpPacketSerializer->Send(Beatmaps::Packets::MpBeatmapPacket::CS_Ctor(localPlayerData->get_beatmapLevel()));
    }
    getLogger().debug("LobbyPlayersDataModel_HandleMenuRpcManagerGetRecommendedBeatmap Finished");

    LobbyPlayersDataModel_HandleMenuRpcManagerGetRecommendedBeatmap(self, userId);
}

MAKE_HOOK_MATCH(LobbyPlayersDataModel_HandleMenuRpcManagerRecommendBeatmap, &LobbyPlayersDataModel::HandleMenuRpcManagerRecommendBeatmap, void, LobbyPlayersDataModel* self, StringW userId, BeatmapIdentifierNetSerializable* beatmapId) {
    if (!Il2CppString::IsNullOrEmpty(Utilities::HashForLevelID(beatmapId->get_levelID())))
        return;

    LobbyPlayersDataModel_HandleMenuRpcManagerRecommendBeatmap(self, userId, beatmapId);
}

MAKE_HOOK_MATCH(LobbyPlayersDataModel_SetLocalPlayerBeatmapLevel, &LobbyPlayersDataModel::SetLocalPlayerBeatmapLevel, void, LobbyPlayersDataModel* self, PreviewDifficultyBeatmap* beatmapLevel) {
    getLogger().debug("Local player selected song '%s'", static_cast<std::string>(beatmapLevel->get_beatmapLevel()->get_levelID()).c_str());
    StringW levelHash = Utilities::HashForLevelID(beatmapLevel->get_beatmapLevel()->get_levelID());
    if (!Il2CppString::IsNullOrEmpty(levelHash))
        mpPacketSerializer->Send(Beatmaps::Packets::MpBeatmapPacket::CS_Ctor(beatmapLevel));

    LobbyPlayersDataModel_SetLocalPlayerBeatmapLevel(self, beatmapLevel);
}

#include "GlobalNamespace/BeatmapIdentifierNetSerializableHelper.hpp"
#include "Beatmaps/NoInfoBeatmapLevel.hpp"
using namespace MultiplayerCore::Beatmaps;
MAKE_HOOK_MATCH(BeatmapIdentifierNetSerializableHelper_ToPreviewDifficultyBeatmap, &BeatmapIdentifierNetSerializableHelper::ToPreviewDifficultyBeatmap, PreviewDifficultyBeatmap*, BeatmapIdentifierNetSerializable* beatmapId, BeatmapLevelsModel* beatmapLevelsModel, BeatmapCharacteristicCollectionSO* beatmapCharacteristicCollection) {
    PreviewDifficultyBeatmap* beatmap = BeatmapIdentifierNetSerializableHelper_ToPreviewDifficultyBeatmap(beatmapId, beatmapLevelsModel, beatmapCharacteristicCollection);
    if (!beatmap->get_beatmapLevel()) beatmap->set_beatmapLevel(reinterpret_cast<IPreviewBeatmapLevel*>(NoInfoBeatmapLevel::New_ctor(Utilities::HashForLevelID(beatmapId->get_levelID()))));
    return beatmap;
}

//MAKE_HOOK_MATCH(LobbyPlayersDataModel_SetPlayerBeatmapLevel, &LobbyPlayersDataModel::SetPlayerBeatmapLevel, void, LobbyPlayersDataModel* self, ::StringW userId, ::GlobalNamespace::PreviewDifficultyBeatmap* beatmapLevel) {
//    if (beatmapLevel && beatmapLevel->get_beatmapLevel())
//        getLogger().debug("LobbyPlayersDataModel_SetPlayerBeatmapLevel: levelId: '%s', songName: '%s', songSubName: '%s', songAuthorName: '%s'",
//            static_cast<std::string>(beatmapLevel->get_beatmapLevel()->get_levelID()).c_str(),
//            static_cast<std::string>(beatmapLevel->get_beatmapLevel()->get_songName()).c_str(),
//            static_cast<std::string>(beatmapLevel->get_beatmapLevel()->get_songSubName()).c_str(),
//            static_cast<std::string>(beatmapLevel->get_beatmapLevel()->get_songAuthorName()).c_str()
//        );
//
//    LobbyPlayersDataModel_SetPlayerBeatmapLevel(self, userId, beatmapLevel);
//}

void MultiplayerCore::Hooks::SessionManagerAndExtendedPlayerHooks() {
    INSTALL_HOOK(getLogger(), SessionManagerStart);
    INSTALL_HOOK(getLogger(), SessionManager_StartSession);

    INSTALL_HOOK(getLogger(), LobbyPlayersDataModel_HandleMenuRpcManagerGetRecommendedBeatmap);
    INSTALL_HOOK(getLogger(), LobbyPlayersDataModel_HandleMenuRpcManagerRecommendBeatmap);
    INSTALL_HOOK(getLogger(), LobbyPlayersDataModel_SetLocalPlayerBeatmapLevel);
    //INSTALL_HOOK(getLogger(), LobbyPlayersDataModel_SetPlayerBeatmapLevel);

    INSTALL_HOOK_ORIG(getLogger(), BeatmapIdentifierNetSerializableHelper_ToPreviewDifficultyBeatmap);
}