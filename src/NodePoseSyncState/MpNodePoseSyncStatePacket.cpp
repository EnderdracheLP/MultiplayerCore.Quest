#include "NodePoseSyncState/MpNodePoseSyncStatePacket.hpp"

DEFINE_TYPE(MultiplayerCore::NodePoseSyncState, MpNodePoseSyncStatePacket);

namespace MultiplayerCore::NodePoseSyncState {
    void MpNodePoseSyncStatePacket::ctor() {
        INVOKE_BASE_CTOR(classof(MultiplayerCore::Networking::Abstractions::MpPacket*));
    }

    void MpNodePoseSyncStatePacket::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
        writer->Put(deltaUpdateFrequencyMs);
        writer->Put(fullStateUpdateFrequencyMs);
    }

    void MpNodePoseSyncStatePacket::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
        deltaUpdateFrequencyMs = reader->GetLong();
        fullStateUpdateFrequencyMs = reader->GetLong();
    }
}
