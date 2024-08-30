#include "Players/MpPlayerData.hpp"
#include "UnityEngine/Application.hpp"

DEFINE_TYPE(MultiplayerCore::Players, MpPlayerData);

namespace MultiplayerCore::Players {
    void MpPlayerData::New() {
        INVOKE_CTOR();
        INVOKE_BASE_CTOR(classof(Networking::Abstractions::MpPacket*));
        std::string version = UnityEngine::Application::get_version();
        std::replace(version.begin(), version.end(), '_', '-');
        gameVersion = version;
    }

    void MpPlayerData::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
        writer->Put(platformId);
        writer->Put((int)platform);
        writer->Put(gameVersion);
    }

    void MpPlayerData::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
        platformId = reader->GetString();
        platform = (Platform)reader->GetInt();
        gameVersion = reader->GetString();
    }
}
