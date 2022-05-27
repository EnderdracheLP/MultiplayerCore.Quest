#pragma once
#include "../Utils/event.hpp"
#include "MpPacketSerializer.hpp"


namespace MultiplayerCore::Networking {
    struct MpNetworkingEvents {

        static event<Networking::MpPacketSerializer*> RegisterPackets;
        static event<Networking::MpPacketSerializer*> UnRegisterPackets;

    };
} 