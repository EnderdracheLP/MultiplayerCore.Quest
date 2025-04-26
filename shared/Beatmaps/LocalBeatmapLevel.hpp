#pragma once

#include "Abstractions/MpBeatmapLevel.hpp"

DECLARE_CLASS_CUSTOM(MultiplayerCore::Beatmaps, LocalBeatmapLevel, Abstractions::MpBeatmapLevel) {
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::BeatmapLevel*, _localLevel);

    DECLARE_CTOR(ctor_2, StringW hash, GlobalNamespace::BeatmapLevel* localLevel);
};
