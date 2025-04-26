#include "../../Networking/Abstractions/MpPacket.hpp"

#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CUSTOM(MultiplayerCore::Players::Packets, MpPerPlayerPacket, MultiplayerCore::Networking::Abstractions::MpPacket) {
    DECLARE_INSTANCE_FIELD(bool, PPDEnabled);
    DECLARE_INSTANCE_FIELD(bool, PPMEnabled);
    
    DECLARE_OVERRIDE_METHOD_MATCH(void, Serialize, &LiteNetLib::Utils::INetSerializable::Serialize, LiteNetLib::Utils::NetDataWriter* writer);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Deserialize, &LiteNetLib::Utils::INetSerializable::Deserialize, LiteNetLib::Utils::NetDataReader* reader);
public:
    DECLARE_DEFAULT_CTOR();
};
