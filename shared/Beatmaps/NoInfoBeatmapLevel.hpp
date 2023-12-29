#pragma once

#include "Abstractions/MpBeatmapLevel.hpp"

DECLARE_CLASS_CUSTOM(MultiplayerCore::Beatmaps, NoInfoBeatmapLevel, Abstractions::MpBeatmapLevel,
    DECLARE_OVERRIDE_METHOD_MATCH(StringW, get_songName, &::GlobalNamespace::IPreviewBeatmapLevel::get_songName);
    DECLARE_OVERRIDE_METHOD_MATCH(StringW, get_songSubName, &::GlobalNamespace::IPreviewBeatmapLevel::get_songSubName);
    DECLARE_OVERRIDE_METHOD_MATCH(StringW, get_songAuthorName, &::GlobalNamespace::IPreviewBeatmapLevel::get_songAuthorName);
    DECLARE_OVERRIDE_METHOD_MATCH(StringW, get_levelAuthorName, &::GlobalNamespace::IPreviewBeatmapLevel::get_levelAuthorName);

    DECLARE_CTOR(ctor_1, StringW hash);
)
