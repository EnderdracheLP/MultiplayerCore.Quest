#pragma once
//#include "main.hpp"
#include "GlobalNamespace/MultiplayerSessionManager.hpp"
#include "GlobalNamespace/LobbyPlayersDataModel.hpp"
#include "GlobalNamespace/LobbyGameStateController.hpp"

namespace MultiplayerCore {
    // Plugin setup stuff
    //extern GlobalNamespace::MultiplayerSessionManager* _multiplayerSessionManager;
    extern GlobalNamespace::LobbyPlayersDataModel* lobbyPlayersDataModel;
    extern GlobalNamespace::LobbyGameStateController* lobbyGameStateController;

    extern StringW getCustomLevelSongPackMaskStr();
}