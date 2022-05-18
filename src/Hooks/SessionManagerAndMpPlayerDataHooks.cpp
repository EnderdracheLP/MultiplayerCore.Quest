#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "Hooks/SessionManagerAndExtendedPlayerHooks.hpp"
#include "GlobalFields.hpp"

#include "UI/CenterScreenLoading.hpp"
#include "Utils/SemVerChecker.hpp"
#include "Utilities.hpp"
#include "CodegenExtensions/EnumUtils.hpp"

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

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"

//Something here could be causing blackscreen bug

using namespace MultiplayerCore;
using namespace GlobalNamespace;

// Handles a HandleMpBeatmapPacket used to transmit data about a custom song.
static void HandleMpBeatmapPacket(Beatmaps::Packets::MpBeatmapPacket* packet, GlobalNamespace::IConnectedPlayer* player) {
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

IPlatformUserModel* platformUserModel;//TODO possible not needed


MAKE_HOOK_MATCH(SessionManagerStart, &MultiplayerSessionManager::Start, void, MultiplayerSessionManager* self) {
    _multiplayerSessionManager = self;
    SessionManagerStart(_multiplayerSessionManager);
}

MAKE_HOOK_MATCH(SessionManager_StartSession, &MultiplayerSessionManager::StartSession, void, MultiplayerSessionManager* self, MultiplayerSessionManager_SessionType sessionType, ConnectedPlayerManager* connectedPlayerManager) {
    SessionManager_StartSession(self, sessionType, connectedPlayerManager);
    if (!mpPacketSerializer) {
        getLogger().debug("Creating MpPacketSerializer");
        mpPacketSerializer = Networking::MpPacketSerializer::New_ctor(_multiplayerSessionManager);

        getLogger().debug("Registering Callback");
        mpPacketSerializer->RegisterCallback<Beatmaps::Packets::MpBeatmapPacket*>(HandleMpBeatmapPacket);
        getLogger().debug("Callback HandleMpBeatmapPacket Registered");
    }
        using namespace MultiplayerCore::Utils;
        self->SetLocalPlayerState("modded", true);
        self->SetLocalPlayerState(getMEStateStr(), MatchesVersion("MappingExtensions", "*"));
        self->SetLocalPlayerState(getNEStateStr(), MatchesVersion("NoodleExtensions", "*"));
        self->SetLocalPlayerState(getChromaStateStr(), MatchesVersion(ChromaID, ChromaVersionRange));
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
    getLogger().debug("BeatmapIdentifierNetSerializableHelper::ToPreviewDifficultyBeatmap");
    PreviewDifficultyBeatmap* beatmap = BeatmapIdentifierNetSerializableHelper_ToPreviewDifficultyBeatmap(beatmapId, beatmapLevelsModel, beatmapCharacteristicCollection);
    if (!(beatmap && beatmap->get_beatmapLevel())) beatmap->set_beatmapLevel(reinterpret_cast<IPreviewBeatmapLevel*>(NoInfoBeatmapLevel::New_ctor(Utilities::HashForLevelID(beatmapId->get_levelID()))));
    return beatmap;
}


void MultiplayerCore::Hooks::SessionManagerAndExtendedPlayerHooks() {
    INSTALL_HOOK(getLogger(), SessionManagerStart);


    INSTALL_HOOK_ORIG(getLogger(), SessionManager_StartSession);

    INSTALL_HOOK(getLogger(), LobbyPlayersDataModel_HandleMenuRpcManagerGetRecommendedBeatmap);
    INSTALL_HOOK(getLogger(), LobbyPlayersDataModel_HandleMenuRpcManagerRecommendBeatmap);
    INSTALL_HOOK(getLogger(), LobbyPlayersDataModel_SetLocalPlayerBeatmapLevel);

    INSTALL_HOOK_ORIG(getLogger(), BeatmapIdentifierNetSerializableHelper_ToPreviewDifficultyBeatmap);
}