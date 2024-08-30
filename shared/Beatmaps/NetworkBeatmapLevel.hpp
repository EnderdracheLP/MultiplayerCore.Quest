#pragma once

#include "Abstractions/MpBeatmapLevel.hpp"
#include "Packets/MpBeatmapPacket.hpp"
#include "beatsaverplusplus/shared/Models/Beatmap.hpp"

DECLARE_CLASS_CUSTOM(MultiplayerCore::Beatmaps, NetworkBeatmapLevel, Abstractions::MpBeatmapLevel,
    DECLARE_INSTANCE_FIELD_PRIVATE(Packets::MpBeatmapPacket*, _packet);

    DECLARE_CTOR(ctor_1, Packets::MpBeatmapPacket*);

    public:
        const std::unordered_map<uint32_t, std::list<std::string>>& get_requirements() { return _packet->requirements; };
        const std::unordered_map<uint32_t, MultiplayerCore::Beatmaps::Abstractions::DifficultyColors>& get_mapColors() { return _packet->mapColors; };
        const std::vector<const MultiplayerCore::Utils::ExtraSongData::Contributor>& get_contributors() { return _packet->contributors; };
)

#undef SONGDOWNLOADER_INCLUDED
