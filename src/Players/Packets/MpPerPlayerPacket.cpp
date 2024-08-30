#include "Players/Packets/MpPerPlayerPacket.hpp"

DEFINE_TYPE(MultiplayerCore::Players::Packets, MpPerPlayerPacket);

namespace MultiplayerCore::Players::Packets {
    void MpPerPlayerPacket::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
        writer->Put(PPDEnabled);
        writer->Put(PPMEnabled);
    }

    void MpPerPlayerPacket::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
        PPDEnabled = reader->GetBool();
        PPMEnabled = reader->GetBool();
    }
}