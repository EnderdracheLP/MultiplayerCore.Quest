#include "ScoreSyncState/MpScoreSyncStatePacket.hpp"
#include "GlobalNamespace/VarIntExtensions.hpp"

DEFINE_TYPE(MultiplayerCore::ScoreSyncState, MpScoreSyncStatePacket);

namespace MultiplayerCore::ScoreSyncState {
    void MpScoreSyncStatePacket::ctor() {
        INVOKE_BASE_CTOR(classof(MultiplayerCore::Networking::Abstractions::MpPacket*));
    }

    void MpScoreSyncStatePacket::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
        GlobalNamespace::VarIntExtensions::PutVarLong(writer, deltaUpdateFrequencyMs);
        GlobalNamespace::VarIntExtensions::PutVarLong(writer, fullStateUpdateFrequencyMs);
    }

    void MpScoreSyncStatePacket::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
        deltaUpdateFrequencyMs = GlobalNamespace::VarIntExtensions::GetVarLong(reader);
        fullStateUpdateFrequencyMs = GlobalNamespace::VarIntExtensions::GetVarLong(reader);
    }
}
