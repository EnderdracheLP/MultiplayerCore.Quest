#pragma once
#include "Players/MpPlayerData.hpp"
#include "Beatmaps/Packets/MpBeatmapPacket.hpp"

extern SafePtr<MultiplayerCore::Players::MpPlayerData> localPlayer;
extern std::map<std::string, SafePtr<MultiplayerCore::Players::MpPlayerData>> _playerData;

static void HandleMpexBeatmapPacket(MultiplayerCore::Beatmaps::Packets::MpBeatmapPacket* packet, GlobalNamespace::IConnectedPlayer* player);