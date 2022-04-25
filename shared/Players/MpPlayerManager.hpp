#pragma once
#include "../Players/MpPlayerData.hpp"
#include "../Utils/event.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"

namespace MultiplayerCore::Players {
    struct MpPlayerManager {
        static event<GlobalNamespace::IConnectedPlayer*, MultiplayerCore::Players::MpPlayerData*> PlayerConnected;

        // C++ equivalent to basegame events
        static event<GlobalNamespace::IConnectedPlayer*> playerConnectedEvent;
        static event<GlobalNamespace::IConnectedPlayer*> playerDisconnectedEvent;
        static event<GlobalNamespace::DisconnectedReason> disconnectedEvent;

        static bool TryGetPlayer(std::string playerId, MultiplayerCore::Players::MpPlayerData*& player);
        static MultiplayerCore::Players::MpPlayerData* GetPlayer(std::string playerId);
    };
}