#pragma once

#include "Networking/Abstractions/MpPacket.hpp"

DECLARE_CLASS_CUSTOM(MultiplayerCore::NodePoseSyncState, MpNodePoseSyncStatePacket, MultiplayerCore::Networking::Abstractions::MpPacket,
    DECLARE_INSTANCE_FIELD(float, deltaUpdateFrequency);
    DECLARE_INSTANCE_FIELD(float, fullStateUpdateFrequency);

    DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&LiteNetLib::Utils::INetSerializable::Serialize>::get(), LiteNetLib::Utils::NetDataWriter* writer);
    DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&LiteNetLib::Utils::INetSerializable::Deserialize>::get(), LiteNetLib::Utils::NetDataReader* reader);

    DECLARE_CTOR(ctor);
)
