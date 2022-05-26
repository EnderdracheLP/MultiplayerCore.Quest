#include "main.hpp"
#include "Players/MpPlayerData.hpp"

DEFINE_TYPE(MultiplayerCore::Players, MpPlayerData)

namespace MultiplayerCore::Players {
#pragma region MpPlayerData
    void MpPlayerData::New() {
        getLogger().debug("Creating MpPlayerData");
    }

    [[nodiscard("Attempting to discard a manually created instance of MpPlayerData")]]
    MpPlayerData* MpPlayerData::Init(StringW platformID, Platform platform)
    {
        auto mpPlayerData = il2cpp_utils::NewSpecific<MpPlayerData*, il2cpp_utils::CreationType::Manual>();

        mpPlayerData->platformId = platformID;
        mpPlayerData->platform = platform;
        getLogger().debug("Creating MpPlayerData finished");
        return mpPlayerData;
    }


    void MpPlayerData::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
        getLogger().debug("MpPlayerData::Serialize");

        writer->Put(platformId);
        writer->Put((int)platform);
        getLogger().debug("Serialize MpPlayerData done");
    }

    void MpPlayerData::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
        getLogger().debug("MpPlayerData::Deserialize");

        this->platformId = reader->GetString();
        this->platform = (Platform)reader->GetInt();
        getLogger().debug("Deserialize MpPlayerData done");
    }
#pragma endregion
}