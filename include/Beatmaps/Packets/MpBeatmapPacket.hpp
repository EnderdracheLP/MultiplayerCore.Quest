#pragma once
#include "custom-types/shared/macros.hpp"
#include "Networking/Abstractions/MpPacket.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/PreviewDifficultyBeatmap.hpp"

DECLARE_CLASS_CUSTOM_DLL(MultiplayerCore::Beatmaps::Packets, MpBeatmapPacket, MultiplayerCore::Networking::Abstractions::MpPacket, "MultiplayerCore.Beatmaps.Packets",
    DECLARE_INSTANCE_FIELD(StringW, levelHash);

    DECLARE_INSTANCE_FIELD(StringW, songName);
    DECLARE_INSTANCE_FIELD(StringW, songSubName);
    DECLARE_INSTANCE_FIELD(StringW, songAuthorName);
    DECLARE_INSTANCE_FIELD(StringW, levelAuthorName);

    DECLARE_INSTANCE_FIELD(float, beatsPerMinute);
    DECLARE_INSTANCE_FIELD(float, songDuration);

    DECLARE_INSTANCE_FIELD(StringW, characteristic);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::BeatmapDifficulty, difficulty);

    DECLARE_DEFAULT_CTOR();

    DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&LiteNetLib::Utils::INetSerializable::Serialize>::get(), LiteNetLib::Utils::NetDataWriter* writer);
    DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&LiteNetLib::Utils::INetSerializable::Deserialize>::get(), LiteNetLib::Utils::NetDataReader* reader);

    public:
        static MpBeatmapPacket* CS_Ctor(GlobalNamespace::PreviewDifficultyBeatmap* beatmap);

        std::unordered_map<uint8_t, std::vector<StringW>> requirements;
)