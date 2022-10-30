#include "Networking/Abstractions/MpPacket.hpp"
#include "custom-types/shared/register.hpp"
using namespace LiteNetLib::Utils;

DEFINE_TYPE(MultiplayerCore::Networking::Abstractions, MpPacket);

namespace MultiplayerCore::Networking::Abstractions {
    void MpPacket::Serialize(NetDataWriter* writer) {}

    void MpPacket::Deserialize(NetDataReader* reader) {}
}