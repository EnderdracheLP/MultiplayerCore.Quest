#include "hooking.hpp"
#include "logging.hpp"
#include "InvokingLock.hpp"

#include "Hooks/NetworkConfigHooks.hpp"
#include "Objects/MpPlayersDataModel.hpp"
// This file exists to override methods on LobbyPlayersDataModel for the MultiplayerCore.Objects.MpPlayersDataModel, since on quest methods are not transformed into virtual calls


#include "GlobalNamespace/NetworkUtility.hpp"
#include "GlobalNamespace/AuthenticationToken.hpp"

using namespace MultiplayerCore::Objects;
// override Activate
MAKE_AUTO_HOOK_ORIG_MATCH(LobbyPlayersDataModel_Activate, &::GlobalNamespace::LobbyPlayersDataModel::Activate, void, GlobalNamespace::LobbyPlayersDataModel* self) {
    INVOKE_LOCK(LobbyPlayersDataModel_Activate);
    if (!lock) {
        LobbyPlayersDataModel_Activate(self);
    } else {
        static auto customKlass = classof(MpPlayersDataModel*);
        if (self->klass == customKlass)
            reinterpret_cast<MpPlayersDataModel*>(self)->Activate_override();
        else
            LobbyPlayersDataModel_Activate(self);
    }
}
// override Deactivate
MAKE_AUTO_HOOK_ORIG_MATCH(LobbyPlayersDataModel_Deactivate, &::GlobalNamespace::LobbyPlayersDataModel::Deactivate, void, GlobalNamespace::LobbyPlayersDataModel* self) {
    INVOKE_LOCK(LobbyPlayersDataModel_Deactivate);
    if (!lock) {
        LobbyPlayersDataModel_Deactivate(self);
    } else {
        static auto customKlass = classof(MpPlayersDataModel*);
        if (self->klass == customKlass)
            reinterpret_cast<MpPlayersDataModel*>(self)->Deactivate_override();
        else
            LobbyPlayersDataModel_Deactivate(self);
    }
}
// override HandleMultiplayerSessionManagerPlayerConnected
MAKE_AUTO_HOOK_ORIG_MATCH(LobbyPlayersDataModel_HandleMultiplayerSessionManagerPlayerConnected, &::GlobalNamespace::LobbyPlayersDataModel::HandleMultiplayerSessionManagerPlayerConnected, void, GlobalNamespace::LobbyPlayersDataModel* self, GlobalNamespace::IConnectedPlayer* connectedPlayer) {
    INVOKE_LOCK(LobbyPlayersDataModel_HandleMultiplayerSessionManagerPlayerConnected);
    if (!lock) {
        LobbyPlayersDataModel_HandleMultiplayerSessionManagerPlayerConnected(self, connectedPlayer);
    } else {
        static auto customKlass = classof(MpPlayersDataModel*);
        if (self->klass == customKlass)
            reinterpret_cast<MpPlayersDataModel*>(self)->HandlePlayerConnected(connectedPlayer);
        else
            LobbyPlayersDataModel_HandleMultiplayerSessionManagerPlayerConnected(self, connectedPlayer);
    }
}

// override HandleMenuRpcManagerGetRecommendedBeatmap
MAKE_AUTO_HOOK_ORIG_MATCH(LobbyPlayersDataModel_HandleMenuRpcManagerGetRecommendedBeatmap, &::GlobalNamespace::LobbyPlayersDataModel::HandleMenuRpcManagerGetRecommendedBeatmap, void, GlobalNamespace::LobbyPlayersDataModel* self, StringW userId) {
    INVOKE_LOCK(LobbyPlayersDataModel_HandleMenuRpcManagerGetRecommendedBeatmap);
    if (!lock) {
        LobbyPlayersDataModel_HandleMenuRpcManagerGetRecommendedBeatmap(self, userId);
    } else {
        static auto customKlass = classof(MpPlayersDataModel*);
        if (self->klass == customKlass)
            reinterpret_cast<MpPlayersDataModel*>(self)->HandleMenuRpcManagerGetRecommendedBeatmap_override(userId);
        else
            LobbyPlayersDataModel_HandleMenuRpcManagerGetRecommendedBeatmap(self, userId);
    }
}
// override HandleMenuRpcManagerRecommendBeatmap
MAKE_AUTO_HOOK_ORIG_MATCH(LobbyPlayersDataModel_HandleMenuRpcManagerRecommendBeatmap, &::GlobalNamespace::LobbyPlayersDataModel::HandleMenuRpcManagerRecommendBeatmap, void, GlobalNamespace::LobbyPlayersDataModel* self, StringW userId, GlobalNamespace::BeatmapKeyNetSerializable* beatmapKey) {
    INVOKE_LOCK(LobbyPlayersDataModel_HandleMenuRpcManagerRecommendBeatmap);
    if (!lock) {
        LobbyPlayersDataModel_HandleMenuRpcManagerRecommendBeatmap(self, userId, beatmapKey);
    } else {
        static auto customKlass = classof(MpPlayersDataModel*);
        if (self->klass == customKlass)
            reinterpret_cast<MpPlayersDataModel*>(self)->HandleMenuRpcManagerRecommendBeatmap_override(userId, beatmapKey);
        else
            LobbyPlayersDataModel_HandleMenuRpcManagerRecommendBeatmap(self, userId, beatmapKey);
    }
}
// override SetLocalPlayerBeatmapLevel
MAKE_AUTO_HOOK_ORIG_MATCH(LobbyPlayersDataModel_SetLocalPlayerBeatmapLevel, &::GlobalNamespace::LobbyPlayersDataModel::SetLocalPlayerBeatmapLevel, void, GlobalNamespace::LobbyPlayersDataModel* self, ByRef<GlobalNamespace::BeatmapKey> key) {
    INVOKE_LOCK(LobbyPlayersDataModel_SetLocalPlayerBeatmapLevel);
    if (!lock) {
        DEBUG("LobbyPlayersDataModel_SetLocalPlayerBeatmapLevel not locked, calling original");
        LobbyPlayersDataModel_SetLocalPlayerBeatmapLevel(self, key);
    } else {
        static auto customKlass = classof(MpPlayersDataModel*);
        if (self->klass == customKlass)
        {
            DEBUG("LobbyPlayersDataModel_SetLocalPlayerBeatmapLevel locked customKlass, calling override");
            reinterpret_cast<MpPlayersDataModel*>(self)->SetLocalPlayerBeatmapLevel_override(key.heldRef);
        }
        else
        {
            LobbyPlayersDataModel_SetLocalPlayerBeatmapLevel(self, key);
        }
    }
}

MAKE_AUTO_HOOK_ORIG_MATCH(NetworkUtility_GetHashedUserId, &::GlobalNamespace::NetworkUtility::GetHashedUserId, StringW, StringW userId, ::GlobalNamespace::AuthenticationToken::Platform platform) {
    // StringW prefix;
    // if (getConfig().config.FindMember("UserIdPrefix") != getConfig().config.MemberEnd() && getConfig().config["UserIdPrefix"].IsString())
    //     prefix = StringW(getConfig().config["UserIdPrefix"].GetString());

    if (MultiplayerCore::Hooks::NetworkConfigHooks::IsOverridingAPI() && (platform == ::GlobalNamespace::AuthenticationToken::Platform::OculusQuest || platform == ::GlobalNamespace::AuthenticationToken::Platform::Oculus)) {
        DEBUG("User is on Quest, using custom prefix");
        DEBUG("Setting userId to hash {}", /*StringW(prefix + */StringW("OculusQuest#" + userId));
        StringW hashedUserId = ::GlobalNamespace::NetworkUtility::GetHashBase64(/*prefix + */StringW("OculusQuest#" + userId));
        DEBUG("Hashed userId is {}", hashedUserId);
        return hashedUserId;
    }
    else {
        return NetworkUtility_GetHashedUserId(/*prefix ? prefix + userId : */userId, platform);
    }
}