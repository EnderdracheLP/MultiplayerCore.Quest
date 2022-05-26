#pragma once
#include "Players/MpPlayerData.hpp"
#include "Beatmaps/Packets/MpBeatmapPacket.hpp"
#include "Networking/MpPacketSerializer.hpp"
#include "Networking/MpNetworkingEvents.hpp"

namespace MultiplayerCore {
    extern MultiplayerCore::Players::MpPlayerData* localPlayer;
    extern std::unordered_map<std::string, MultiplayerCore::Players::MpPlayerData*> _playerData;


    extern void HandlePlayerConnected(GlobalNamespace::IConnectedPlayer* player);
    extern void HandlePlayerDisconnected(GlobalNamespace::IConnectedPlayer* player);
    extern void HandleDisconnect(GlobalNamespace::DisconnectedReason reason);

    extern void HandleRegisterMpPackets(MultiplayerCore::Networking::MpPacketSerializer* _mpPacketSerializer);

    static void HandleMpBeatmapPacket(MultiplayerCore::Beatmaps::Packets::MpBeatmapPacket* packet, GlobalNamespace::IConnectedPlayer* player);
}