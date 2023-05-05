#pragma once

#include "Abstractions/MpBeatmapLevel.hpp"
#include "songdownloader/shared/Types/BeatSaver/Beatmap.hpp"

DECLARE_CLASS_CUSTOM(MultiplayerCore::Beatmaps, BeatSaverBeatmapLevel, Abstractions::MpBeatmapLevel,
	DECLARE_OVERRIDE_METHOD(StringW, get_songName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songName>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songSubName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songSubName>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songAuthorName>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_levelAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_levelAuthorName>::get());
	DECLARE_OVERRIDE_METHOD(float, get_beatsPerMinute, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_beatsPerMinute>::get());
	DECLARE_OVERRIDE_METHOD(float, get_songDuration, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songDuration>::get());

    DECLARE_CTOR(ctor_1, StringW hash);
    public:
        static BeatSaverBeatmapLevel* Make(const std::string& hash, const BeatSaver::Beatmap& beatmap);
    protected:
        BeatSaver::Beatmap beatmap;
)
