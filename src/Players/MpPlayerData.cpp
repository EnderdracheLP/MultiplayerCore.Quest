#include "Players/MpPlayerData.hpp"

DEFINE_TYPE(MultiplayerCore::Players, MpPlayerData);

namespace MultiplayerCore::Players {
    void MpPlayerData::New() {
        INVOKE_CTOR();
        INVOKE_BASE_CTOR(classof(Networking::Abstractions::MpPacket*));
    }

    void MpPlayerData::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
        writer->Put(platformId);
        writer->Put((int)platform);
    }

    void MpPlayerData::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
        platformId = reader->GetString();
        platform = (Platform)reader->GetInt();
    }
}
