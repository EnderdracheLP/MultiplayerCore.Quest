#include "NodePoseSyncState/MpNodePoseSyncStatePacket.hpp"
#include "GlobalNamespace/VarIntExtensions.hpp"

DEFINE_TYPE(MultiplayerCore::NodePoseSyncState, MpNodePoseSyncStatePacket);

namespace MultiplayerCore::NodePoseSyncState {
    void MpNodePoseSyncStatePacket::ctor() {
        INVOKE_BASE_CTOR(classof(MultiplayerCore::Networking::Abstractions::MpPacket*));
    }

    void MpNodePoseSyncStatePacket::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
        GlobalNamespace::VarIntExtensions::PutVarLong(writer, deltaUpdateFrequencyMs);
        GlobalNamespace::VarIntExtensions::PutVarLong(writer, fullStateUpdateFrequencyMs);
    }

    void MpNodePoseSyncStatePacket::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
        deltaUpdateFrequencyMs = GlobalNamespace::VarIntExtensions::GetVarLong(reader);
        fullStateUpdateFrequencyMs = GlobalNamespace::VarIntExtensions::GetVarLong(reader);
    }
}
