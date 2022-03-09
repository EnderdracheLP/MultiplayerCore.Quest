#include "Players/MpPlayerData.hpp"
#include "CodegenExtensions/ColorUtility.hpp"

DEFINE_TYPE(MultiplayerCore::Players, MpPlayerData)

namespace MultiplayerCore::Players {

    //static Il2CppString* MQE_Protocol() {
    //    static Il2CppString* protocol = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>(MPEX_PROTOCOL);
    //    return protocol;
    //}

#pragma region ExtendedPlayer
    void MpPlayerData::New() {
        getLogger().debug("Creating MpPlayerData");
    }

    MpPlayerData* MpPlayerData::Init(StringW platformID, Platform platform)
    {
        auto mpPlayerData = THROW_UNLESS(il2cpp_utils::New<MpPlayerData*>());

        mpPlayerData->platformID = platformID;
        mpPlayerData->platform = (Platform)platform;
        getLogger().debug("Creating MpPlayerData finished");
        return mpPlayerData;
    }


    void MpPlayerData::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
        getLogger().debug("MpPlayerData::Serialize");

        writer->Put(platformID);
        writer->Put((int)platform);
        getLogger().debug("Serialize MpPlayerData done");
    }

    void MpPlayerData::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
        getLogger().debug("MpPlayerData::Deserialize");

        this->platformID = reader->GetString();
        this->platform = (Platform)reader->GetInt();
        getLogger().debug("Deserialize MpPlayerData done");
    }
#pragma endregion
}