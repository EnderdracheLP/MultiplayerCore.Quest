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
#include "Networking/MpNetworkingEvents.hpp"

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
#include "shared/Utils/event.hpp"
#include "shared/Players/MpPlayerManager.hpp"
#include "Players/MpPlayerData.hpp"


//Something here could be causing blackscreen bug

using namespace MultiplayerCore;
using namespace GlobalNamespace;

MultiplayerCore::Players::MpPlayerData* localPlayer;
std::unordered_map<std::string, MultiplayerCore::Players::MpPlayerData*> _playerData;

event<GlobalNamespace::DisconnectedReason> MultiplayerCore::Players::MpPlayerManager::disconnectedEvent;
event<GlobalNamespace::IConnectedPlayer*> MultiplayerCore::Players::MpPlayerManager::playerConnectedEvent;
event<GlobalNamespace::IConnectedPlayer*> MultiplayerCore::Players::MpPlayerManager::playerDisconnectedEvent;

event<GlobalNamespace::IConnectedPlayer*, MultiplayerCore::Players::MpPlayerData*> MultiplayerCore::Players::MpPlayerManager::ReceivedPlayerData;

event<MultiplayerCore::Networking::MpPacketSerializer*> MultiplayerCore::Networking::MpNetworkingEvents::RegisterPackets;
event_handler<MultiplayerCore::Networking::MpPacketSerializer*> _RegisterMpPacketsHandler = MultiplayerCore::event_handler<MultiplayerCore::Networking::MpPacketSerializer*>(HandleRegisterMpPackets);


event_handler<GlobalNamespace::IConnectedPlayer*> _PlayerConnectedHandler = MultiplayerCore::event_handler<GlobalNamespace::IConnectedPlayer*>(HandlePlayerConnected);
event_handler<GlobalNamespace::IConnectedPlayer*> _PlayerDisconnectedHandler = MultiplayerCore::event_handler<GlobalNamespace::IConnectedPlayer*>(HandlePlayerDisconnected);
event_handler<GlobalNamespace::DisconnectedReason> _DisconnectedHandler = MultiplayerCore::event_handler<GlobalNamespace::DisconnectedReason>(HandleDisconnect);

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


bool Players::MpPlayerManager::TryGetMpPlayerData(std::string playerId, Players::MpPlayerData*& player) {
    if (_playerData.find(playerId) != _playerData.end()) {
        player = _playerData.at(playerId);
        return true;
    }
    return false;
}

Players::MpPlayerData* Players::MpPlayerManager::GetMpPlayerData(std::string playerId) {
    if (_playerData.find(playerId) != _playerData.end()) {
        return _playerData.at(playerId);
    }
    return nullptr;
}

static void HandlePlayerData(Players::MpPlayerData* playerData, IConnectedPlayer* player) {
    if(player){
        const std::string userId = to_utf8(csstrtostr(player->get_userId()));
        if (_playerData.contains(userId)) {
            getLogger().debug("HandlePlayerData, player already exists");
            _playerData.at(userId) = playerData;
        }
        else {
            getLogger().info("Received new 'MpPlayerData' from '%s' with platformID: '%s' platform: '%d'",
                static_cast<std::string>(player->get_userId()).c_str(),
                static_cast<std::string>(playerData->platformId).c_str(),
                (int)playerData->platform
            );
            _playerData.emplace(userId, playerData);
        }
        getLogger().debug("MpPlayerData firing event");
        Players::MpPlayerManager::ReceivedPlayerData(player, playerData);
        getLogger().debug("MpPlayerData done");
    }
}

void HandleRegisterMpPackets(MultiplayerCore::Networking::MpPacketSerializer* _mpPacketSerializer) {
        _mpPacketSerializer->RegisterCallback<Players::MpPlayerData*>(HandlePlayerData);
        getLogger().debug("Callback MpPlayerDataPacket Registered");

        _mpPacketSerializer->RegisterCallback<Beatmaps::Packets::MpBeatmapPacket*>(HandleMpBeatmapPacket);
        getLogger().debug("Callback HandleMpBeatmapPacket Registered");
}

void HandlePlayerConnected(IConnectedPlayer* player) {
    try {
        getLogger().debug("MPCore HandlePlayerConnected");
        if (player) {
            getLogger().info("Player '%s' joined", static_cast<std::string>(player->get_userId()).c_str());
            if (localPlayer->platformId)
            {
                getLogger().debug("Sending MpPlayerData with platformID: '%s' platform: '%d'",
                    static_cast<std::string>(localPlayer->platformId).c_str(),
                    (int)localPlayer->platform
                );
                MultiplayerCore::mpPacketSerializer->Send(localPlayer);
            }
            getLogger().debug("MpPlayerData sent");
        }
    }
    catch (const std::runtime_error& e) {
        getLogger().error("REPORT TO ENDER: %s", e.what());
    }
}

void HandlePlayerDisconnected(IConnectedPlayer* player) {
    try {
        getLogger().debug("MPCore HandlePlayerDisconnected");
        if (player) {
                const std::string userId = to_utf8(csstrtostr(player->get_userId()));
                getLogger().info("Player '%s' left", userId.c_str());
            if(_playerData.contains(userId)){
                _playerData.erase(userId);
            }
        }
    }
        catch (const std::runtime_error& e) {
        getLogger().error("REPORT TO ENDER: %s", e.what());
    }
}


void HandleDisconnect(DisconnectedReason reason) {
    getLogger().info("Disconnected from server reason: '%s'", MultiplayerCore::EnumUtils::GetEnumName(reason).c_str());
    _playerData.clear();
    getLogger().info("MPCore Removing connected/disconnected/disconnect events");
    Players::MpPlayerManager::playerConnectedEvent -= _PlayerConnectedHandler;
    Players::MpPlayerManager::playerDisconnectedEvent -= _PlayerDisconnectedHandler;
    Players::MpPlayerManager::disconnectedEvent -= _DisconnectedHandler;
}


MAKE_HOOK_MATCH(LobbyPlayersDataModel_sHandleMultiplayerSessionManagerPlayerConnected, &LobbyPlayersDataModel::HandleMultiplayerSessionManagerPlayerConnected, void, LobbyPlayersDataModel* self, IConnectedPlayer* player) {
    getLogger().debug("LobbyPlayersDataModel_sHandleMultiplayerSessionManagerPlayerConnected");
    LobbyPlayersDataModel_sHandleMultiplayerSessionManagerPlayerConnected(self, player);
    getLogger().debug("LobbyPlayersDataModel_sHandleMultiplayerSessionManagerPlayerConnected, triggering MQE event");
    Players::MpPlayerManager::playerConnectedEvent(player);
}

MAKE_HOOK_MATCH(LobbyPlayersDataModel_HandleMultiplayerSessionManagerPlayerDisconnected, &LobbyPlayersDataModel::HandleMultiplayerSessionManagerPlayerDisconnected, void, LobbyPlayersDataModel* self, IConnectedPlayer* player) {
    getLogger().debug("LobbyPlayersDataModel_HandleMultiplayerSessionManagerPlayerDisconnected");
    LobbyPlayersDataModel_HandleMultiplayerSessionManagerPlayerDisconnected(self, player);
    getLogger().debug("LobbyPlayersDataModel_HandleMultiplayerSessionManagerPlayerDisconnected, triggering MQE event");
    Players::MpPlayerManager::playerDisconnectedEvent(player);
}

MAKE_HOOK_MATCH(LobbyGameStateController_HandleMultiplayerSessionManagerDisconnected, &LobbyGameStateController::HandleMultiplayerSessionManagerDisconnected, void, LobbyGameStateController* self, DisconnectedReason disconnectedReason) {
    getLogger().debug("LobbyGameStateController_HandleMultiplayerSessionManagerDisconnected");
    LobbyGameStateController_HandleMultiplayerSessionManagerDisconnected(self, disconnectedReason);
    Players::MpPlayerManager::disconnectedEvent(disconnectedReason);
}


Players::Platform getPlatform(UserInfo::Platform platform) {
    switch (platform.value) {
    case UserInfo::Platform::Oculus:
        getLogger().debug("Platform: Oculus = OculusQuest");
        return Players::Platform::OculusQuest; // If platform is Oculus, we assume the user is using Quest
    case UserInfo::Platform::PS4:
        getLogger().debug("Platform: PS4");
        return Players::Platform::PS4;
    case UserInfo::Platform::Steam:
        getLogger().debug("Platform: Steam");
        return Players::Platform::Steam;
    case UserInfo::Platform::Test:
        getLogger().debug("Platform: Test = Unknown");
        return Players::Platform::Unknown;
    default:
        try {
            getLogger().debug("Platform: %s", MultiplayerCore::EnumUtils::GetEnumName(platform).c_str());
        }
        catch (const std::runtime_error& e) {
            getLogger().error("REPORT TO ENDER: %s", e.what());
        }
        catch (const std::exception& e) {
            getLogger().error("REPORT TO ENDER: %s", e.what());
        }
        catch (...) {
            getLogger().error("REPORT TO ENDER: Unknown exception");
        }
        getLogger().debug("Platform: %d", (int)platform.value);
        return (Players::Platform)platform.value;
    }
}

MAKE_HOOK_MATCH(SessionManagerStart, &MultiplayerSessionManager::Start, void, MultiplayerSessionManager* self) {
    _multiplayerSessionManager = self;
    SessionManagerStart(_multiplayerSessionManager);
}

MAKE_HOOK_MATCH(SessionManager_StartSession, &MultiplayerSessionManager::StartSession, void, MultiplayerSessionManager* self, MultiplayerSessionManager_SessionType sessionType, ConnectedPlayerManager* connectedPlayerManager) {
    SessionManager_StartSession(self, sessionType, connectedPlayerManager);
    
    getLogger().debug("MultiplayerSessionManager.StartSession, creating localPlayer");
    static auto localNetworkPlayerModel = UnityEngine::Resources::FindObjectsOfTypeAll<LocalNetworkPlayerModel*>().get(0);
    static auto UserInfoTask = localNetworkPlayerModel->dyn__platformUserModel()->GetUserInfo();
    static auto action = il2cpp_utils::MakeDelegate<System::Action_1<System::Threading::Tasks::Task*>*>(classof(System::Action_1<System::Threading::Tasks::Task*>*), (std::function<void(System::Threading::Tasks::Task_1<GlobalNamespace::UserInfo*>*)>)[&](System::Threading::Tasks::Task_1<GlobalNamespace::UserInfo*>* userInfoTask) {
        auto userInfo = userInfoTask->get_Result();
        if (userInfo) {
            if (!localPlayer) localPlayer = Players::MpPlayerData::Init(userInfo->dyn_platformUserId(), getPlatform(userInfo->dyn_platform()));
            else {
                localPlayer->platformId = userInfo->dyn_platformUserId();
                localPlayer->platform = getPlatform(userInfo->dyn_platform());
            }
        }
        else getLogger().error("Failed to get local network player!");
        }
    );
        if(action){
    reinterpret_cast<System::Threading::Tasks::Task*>(UserInfoTask)->ContinueWith(action);
    action = nullptr;
    }


    if (!mpPacketSerializer) {
        getLogger().debug("Creating MpPacketSerializer and calling register event");
        mpPacketSerializer = Networking::MpPacketSerializer::New_ctor<il2cpp_utils::CreationType::Manual>(_multiplayerSessionManager);
        MultiplayerCore::Networking::MpNetworkingEvents::RegisterPackets(mpPacketSerializer);
    }

    using namespace MultiplayerCore::Utils;
    self->SetLocalPlayerState("modded", true);
    self->SetLocalPlayerState(getMEStateStr(), MatchesVersion("MappingExtensions", "*"));
    self->SetLocalPlayerState(getNEStateStr(), MatchesVersion("NoodleExtensions", "*"));
    self->SetLocalPlayerState(getChromaStateStr(), MatchesVersion(ChromaID, ChromaVersionRange));


    Players::MpPlayerManager::playerConnectedEvent += _PlayerConnectedHandler;
    Players::MpPlayerManager::playerDisconnectedEvent += _PlayerDisconnectedHandler;
    Players::MpPlayerManager::disconnectedEvent += _DisconnectedHandler;
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
    MultiplayerCore::Networking::MpNetworkingEvents::RegisterPackets += _RegisterMpPacketsHandler;
    INSTALL_HOOK(getLogger(), SessionManagerStart);


    INSTALL_HOOK_ORIG(getLogger(), SessionManager_StartSession);

    INSTALL_HOOK(getLogger(), LobbyPlayersDataModel_sHandleMultiplayerSessionManagerPlayerConnected);
    INSTALL_HOOK(getLogger(), LobbyPlayersDataModel_HandleMultiplayerSessionManagerPlayerDisconnected);
    INSTALL_HOOK(getLogger(), LobbyGameStateController_HandleMultiplayerSessionManagerDisconnected);

    INSTALL_HOOK(getLogger(), LobbyPlayersDataModel_HandleMenuRpcManagerGetRecommendedBeatmap);
    INSTALL_HOOK(getLogger(), LobbyPlayersDataModel_HandleMenuRpcManagerRecommendBeatmap);
    INSTALL_HOOK(getLogger(), LobbyPlayersDataModel_SetLocalPlayerBeatmapLevel);

    INSTALL_HOOK_ORIG(getLogger(), BeatmapIdentifierNetSerializableHelper_ToPreviewDifficultyBeatmap);
}