#include "hooking.hpp"
#include "logging.hpp"

#include "GlobalNamespace/MenuRpcManager.hpp"
#include "GlobalNamespace/ConnectedPlayerManager.hpp"
#include "GlobalNamespace/EntitlementsStatus.hpp"
#include "GlobalNamespace/CannotStartGameReason.hpp"
#include "GlobalNamespace/LobbyGameStateController.hpp"
#include "GlobalNamespace/PlayersMissingEntitlementsNetSerializable.hpp"
#include "GlobalNamespace/IMultiplayerSessionManager.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"

#include "LiteNetLib/Utils/NetDataWriter.hpp"
#include "LiteNetLib/Utils/INetSerializable.hpp"

static inline std::string_view EntitlementName(GlobalNamespace::EntitlementsStatus entitlement) {
    switch (entitlement) {
        case GlobalNamespace::EntitlementsStatus::Unknown:
            return "Unknown";
        case GlobalNamespace::EntitlementsStatus::NotOwned:
            return "NotOwned";
        case GlobalNamespace::EntitlementsStatus::NotDownloaded:
            return "NotDownloaded";
        case GlobalNamespace::EntitlementsStatus::Ok:
            return "Ok";
        default:
            return "Null";
    }
}

static inline std::string_view CannotStartGameReasonName(GlobalNamespace::CannotStartGameReason reason) {
    switch (reason) {
        case GlobalNamespace::CannotStartGameReason::None:
            return "None";
        case GlobalNamespace::CannotStartGameReason::AllPlayersSpectating:
            return "AllPlayersSpectating";
        case GlobalNamespace::CannotStartGameReason::NoSongSelected:
            return "NoSongSelected";
        case GlobalNamespace::CannotStartGameReason::AllPlayersNotInLobby:
            return "AllPlayersNotInLobby";
        case GlobalNamespace::CannotStartGameReason::DoNotOwnSong:
            return "DoNotOwnSong";
    }
}


MAKE_AUTO_HOOK_MATCH(MenuRpcManager_SetIsStartButtonEnabled, &GlobalNamespace::MenuRpcManager::InvokeSetIsStartButtonEnabled, void, GlobalNamespace::MenuRpcManager* self, ::StringW userId, ::GlobalNamespace::CannotStartGameReason reason) {
    DEBUG("Setting start button enabled from {} with reason {}", userId, CannotStartGameReasonName(reason));
    MenuRpcManager_SetIsStartButtonEnabled(self, userId, reason);
}

MAKE_AUTO_HOOK_MATCH(MenuRpcManager_SetIsEntitledToLevel, &GlobalNamespace::MenuRpcManager::SetIsEntitledToLevel, void, GlobalNamespace::MenuRpcManager* self, ::StringW levelId, ::GlobalNamespace::EntitlementsStatus entitlementStatus) {
    DEBUG("Setting entitlement for level {} to {}", levelId, EntitlementName(entitlementStatus));
    MenuRpcManager_SetIsEntitledToLevel(self, levelId, entitlementStatus);
}

MAKE_AUTO_HOOK_MATCH(MenuRpcManager_InvokeSetPlayersMissingEntitlementsToLevelRpc, &GlobalNamespace::MenuRpcManager::InvokeSetPlayersMissingEntitlementsToLevelRpc, void, GlobalNamespace::MenuRpcManager* self, ::StringW userId, ::GlobalNamespace::PlayersMissingEntitlementsNetSerializable* playersMissingEntitlements) {
    INFO("Removing local player id from list of players missing entitlement");
    // playersMissingEntitlements->playersWithoutEntitlements->Remove(self->multiplayerSessionManager->localPlayer->userId);

    MenuRpcManager_InvokeSetPlayersMissingEntitlementsToLevelRpc(self, userId, playersMissingEntitlements);
}

MAKE_AUTO_HOOK_MATCH(ConnectedPlayerManager_Write, &GlobalNamespace::ConnectedPlayerManager::Write, void, GlobalNamespace::ConnectedPlayerManager* self, LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet) {
    auto obj = (Il2CppObject*)packet;

    if (self->_reliableDataWriter == writer) {
        if (packet) [[likely]] INFO("[Reliable Send] Sending packet of type {}::{}", obj->klass->namespaze, obj->klass->name);
        else INFO("[Reliable Send] Sending nullptr packet?");
    } else if (self->_temporaryDataWriter == writer) {
        if (packet) [[likely]] INFO("[Temporary Send] Sending packet of type {}::{}", obj->klass->namespaze, obj->klass->name);
        else INFO("[Temporary Send] Sending nullptr packet?");
    } else if (self->_unreliableDataWriter == writer) {
        if (packet) [[likely]] INFO("[Unreliable Send] Sending packet of type {}::{}", obj->klass->namespaze, obj->klass->name);
        else INFO("[Unreliable Send] Sending nullptr packet?");
    } else if (self->_temporaryEncryptedDataWriter == writer) {
        if (packet) [[likely]] INFO("[Temporary Encrypted Send] Sending packet of type {}::{}", obj->klass->namespaze, obj->klass->name);
        else INFO("[Temporary Encrypted Send] Sending nullptr packet?");
    }

    ConnectedPlayerManager_Write(self, writer, packet);
}
