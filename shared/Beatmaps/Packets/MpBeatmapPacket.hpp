#pragma once

#include "custom-types/shared/macros.hpp"
#include "../../Networking/Abstractions/MpPacket.hpp"
#include "../../Utils/ExtraSongData.hpp"
#include "../Abstractions/DifficultyColors.hpp"

#include "GlobalNamespace/BeatmapLevel.hpp"
#include "GlobalNamespace/BeatmapKey.hpp"
#include <list>

DECLARE_CLASS_CUSTOM(MultiplayerCore::Beatmaps::Packets, MpBeatmapPacket, MultiplayerCore::Networking::Abstractions::MpPacket,
    DECLARE_INSTANCE_FIELD(StringW, levelHash);
    DECLARE_INSTANCE_FIELD(StringW, songName);
    DECLARE_INSTANCE_FIELD(StringW, songSubName);
    DECLARE_INSTANCE_FIELD(StringW, songAuthorName);
    DECLARE_INSTANCE_FIELD(StringW, levelAuthorName);
    DECLARE_INSTANCE_FIELD(float, beatsPerMinute);
    DECLARE_INSTANCE_FIELD(float, songDuration);

    DECLARE_INSTANCE_FIELD(StringW, characteristic);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::BeatmapDifficulty, difficulty);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Serialize, &LiteNetLib::Utils::INetSerializable::Serialize, LiteNetLib::Utils::NetDataWriter* writer);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Deserialize, &LiteNetLib::Utils::INetSerializable::Deserialize, LiteNetLib::Utils::NetDataReader* reader);

    DECLARE_CTOR(New);
    public:
        static MpBeatmapPacket* New_1(GlobalNamespace::BeatmapLevel* beatmap, GlobalNamespace::BeatmapKey beatmapKey);
        std::unordered_map<uint32_t, std::list<std::string>> requirements;
        std::unordered_map<uint32_t, MultiplayerCore::Beatmaps::Abstractions::DifficultyColors> mapColors;
        std::vector<const MultiplayerCore::Utils::ExtraSongData::Contributor> contributors;
)
