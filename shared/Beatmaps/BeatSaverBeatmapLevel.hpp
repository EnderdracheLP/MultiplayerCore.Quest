#pragma once

#include "Abstractions/MpBeatmapLevel.hpp"
#include "beatsaverplusplus/shared/Models/Beatmap.hpp"

DECLARE_CLASS_CUSTOM(MultiplayerCore::Beatmaps, BeatSaverBeatmapLevel, Abstractions::MpBeatmapLevel) {
    DECLARE_CTOR(ctor_1, StringW hash);
    public:
        static BeatSaverBeatmapLevel* Make(const std::string& hash, const BeatSaver::Models::Beatmap& beatmap);
    protected:
        BeatSaver::Models::Beatmap beatmap;
};
