#include "hooking.hpp"
#include "logging.hpp"
#include "InvokingLock.hpp"

#include "Objects/MpPlayersDataModel.hpp"
#include "GlobalNamespace/BeatmapIdentifierNetSerializable.hpp"
#include "GlobalNamespace/PreviewDifficultyBeatmap.hpp"
// This file exists to override methods on LobbyPlayersDataModel for the MultiplayerCore.Objects.MpPlayersDataModel, since on quest methods are not transformed into virtual calls

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
MAKE_AUTO_HOOK_ORIG_MATCH(LobbyPlayersDataModel_HandleMenuRpcManagerRecommendBeatmap, &::GlobalNamespace::LobbyPlayersDataModel::HandleMenuRpcManagerRecommendBeatmap, void, GlobalNamespace::LobbyPlayersDataModel* self, StringW userId, GlobalNamespace::BeatmapIdentifierNetSerializable* beatmapId) {
    INVOKE_LOCK(LobbyPlayersDataModel_HandleMenuRpcManagerRecommendBeatmap);
    if (!lock) {
        LobbyPlayersDataModel_HandleMenuRpcManagerRecommendBeatmap(self, userId, beatmapId);
    } else {
        static auto customKlass = classof(MpPlayersDataModel*);
        if (self->klass == customKlass)
            reinterpret_cast<MpPlayersDataModel*>(self)->HandleMenuRpcManagerRecommendBeatmap_override(userId, beatmapId);
        else
            LobbyPlayersDataModel_HandleMenuRpcManagerRecommendBeatmap(self, userId, beatmapId);
    }
}
// override SetLocalPlayerBeatmapLevel
MAKE_AUTO_HOOK_ORIG_MATCH(LobbyPlayersDataModel_SetLocalPlayerBeatmapLevel, &::GlobalNamespace::LobbyPlayersDataModel::SetLocalPlayerBeatmapLevel, void, GlobalNamespace::LobbyPlayersDataModel* self, GlobalNamespace::PreviewDifficultyBeatmap* beatmapLevel) {
    INVOKE_LOCK(LobbyPlayersDataModel_SetLocalPlayerBeatmapLevel);
    if (!lock) {
        LobbyPlayersDataModel_SetLocalPlayerBeatmapLevel(self, beatmapLevel);
    } else {
        static auto customKlass = classof(MpPlayersDataModel*);
        if (self->klass == customKlass)
            reinterpret_cast<MpPlayersDataModel*>(self)->SetLocalPlayerBeatmapLevel_override(beatmapLevel);
        else
            LobbyPlayersDataModel_SetLocalPlayerBeatmapLevel(self, beatmapLevel);
    }
}
