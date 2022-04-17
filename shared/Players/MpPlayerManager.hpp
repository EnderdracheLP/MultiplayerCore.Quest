#pragma once
#include "../Players/MpPlayerData.hpp"
#include "../Utils/event.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"

namespace MultiplayerCore::Players {
    struct MpPlayerManager {
        static event<GlobalNamespace::IConnectedPlayer*, MultiplayerCore::Players::MpPlayerData*> PlayerConnected;
        static bool TryGetPlayer(std::string playerId, MultiplayerCore::Players::MpPlayerData*& player);
        static MultiplayerCore::Players::MpPlayerData* GetPlayer(std::string playerId);
    };
}