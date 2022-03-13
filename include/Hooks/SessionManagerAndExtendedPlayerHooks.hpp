#pragma once
//#include "Extensions/ExtendedPlayer.hpp"
#include "Players/MpPlayerData.hpp"
//#include "Beatmaps/Packets/MpBeatmapPacket.hpp"

extern SafePtr<MultiplayerCore::Players::MpPlayerData> localPlayer;
extern std::map<std::string, SafePtr<MultiplayerCore::Players::MpPlayerData>> _players;
//extern SafePtr<MultiQuestensions::Extensions::ExtendedPlayer> localExtendedPlayer;
//extern std::map<std::string, SafePtr<MultiQuestensions::Extensions::ExtendedPlayer>> _extendedPlayers;

//static void HandleMpexBeatmapPacket(MultiplayerCore::Beatmaps::Packet::MpexBeatmapPacket* packet, GlobalNamespace::IConnectedPlayer* player);