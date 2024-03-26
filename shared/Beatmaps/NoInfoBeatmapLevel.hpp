#pragma once

#include "Abstractions/MpBeatmapLevel.hpp"

DECLARE_CLASS_CUSTOM(MultiplayerCore::Beatmaps, NoInfoBeatmapLevel, Abstractions::MpBeatmapLevel,
    DECLARE_CTOR(ctor_1, StringW hash);
)
