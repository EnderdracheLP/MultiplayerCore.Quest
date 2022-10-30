#pragma once
#include "../Players/MpPlayerData.hpp"
#include "../Utils/event.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"

namespace MultiplayerCore::Players {
    struct MpPlayerManager {
        static event<GlobalNamespace::IConnectedPlayer*, MultiplayerCore::Players::MpPlayerData*> ReceivedPlayerData;
        // C++ equivalent to basegame events
        static event<GlobalNamespace::IConnectedPlayer*> connectingEvent;//When the local player is connecting, This is ran before the player joins the lobby
        static event<GlobalNamespace::DisconnectedReason, GlobalNamespace::IConnectedPlayer*> disconnectedEvent;//When the local player disconnects from the lobby
        static event<GlobalNamespace::IConnectedPlayer*> playerConnectedEvent;//When a remote player joins
        static event<GlobalNamespace::IConnectedPlayer*> playerDisconnectedEvent;//when a remote player leaves
        

        static bool TryGetMpPlayerData(std::string const& playerId, MultiplayerCore::Players::MpPlayerData*& player);
        static MultiplayerCore::Players::MpPlayerData* GetMpPlayerData(std::string const& playerId);
    };
} 