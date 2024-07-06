#pragma once

#include "Networking/Abstractions/MpPacket.hpp"

DECLARE_CLASS_CUSTOM(MultiplayerCore::ScoreSyncState, MpScoreSyncStatePacket, MultiplayerCore::Networking::Abstractions::MpPacket,
    DECLARE_INSTANCE_FIELD(long, deltaUpdateFrequencyMs);
    DECLARE_INSTANCE_FIELD(long, fullStateUpdateFrequencyMs);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Serialize, &LiteNetLib::Utils::INetSerializable::Serialize, LiteNetLib::Utils::NetDataWriter* writer);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Deserialize, &LiteNetLib::Utils::INetSerializable::Deserialize, LiteNetLib::Utils::NetDataReader* reader);

    DECLARE_CTOR(ctor);
)
