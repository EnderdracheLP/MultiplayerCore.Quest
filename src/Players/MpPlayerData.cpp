#include "Players/MpPlayerData.hpp"

DEFINE_TYPE(MultiplayerCore::Players, MpPlayerData);

namespace MultiplayerCore::Players {
    void MpPlayerData::New() {}

    void MpPlayerData::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
        writer->Put(platformId);
        writer->Put((int)platform);
    }

    void MpPlayerData::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
        platformId = reader->GetString();
        platform = (Platform)reader->GetInt();
    }

    // TODO: is this really needed? it seems sus...
    MpPlayerData* MpPlayerData::Init(StringW platformID, Platform platform) {
        auto data = il2cpp_utils::NewSpecific<MpPlayerData*, il2cpp_utils::CreationType::Manual>();
        data->platformId = platformID;
        data->platform = platform;
        return data;
    }
}
