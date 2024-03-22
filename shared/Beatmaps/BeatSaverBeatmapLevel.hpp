#pragma once

#include "Abstractions/MpBeatmapLevel.hpp"
#include "songdownloader/shared/Types/BeatSaver/Beatmap.hpp"

DECLARE_CLASS_CUSTOM(MultiplayerCore::Beatmaps, BeatSaverBeatmapLevel, Abstractions::MpBeatmapLevel,
	DECLARE_INSTANCE_FIELD_PRIVATE(::System::Threading::Tasks::Task_1<::UnityEngine::Sprite*>*, _coverImageTask);
	DECLARE_OVERRIDE_METHOD_MATCH(StringW, get_songName, &GlobalNamespace::IPreviewBeatmapLevel::get_songName);
	DECLARE_OVERRIDE_METHOD_MATCH(StringW, get_songSubName, &GlobalNamespace::IPreviewBeatmapLevel::get_songSubName);
	DECLARE_OVERRIDE_METHOD_MATCH(StringW, get_songAuthorName, &GlobalNamespace::IPreviewBeatmapLevel::get_songAuthorName);
	DECLARE_OVERRIDE_METHOD_MATCH(StringW, get_levelAuthorName, &GlobalNamespace::IPreviewBeatmapLevel::get_levelAuthorName);
	DECLARE_OVERRIDE_METHOD_MATCH(float, get_beatsPerMinute, &GlobalNamespace::IPreviewBeatmapLevel::get_beatsPerMinute);
	DECLARE_OVERRIDE_METHOD_MATCH(float, get_songDuration, &GlobalNamespace::IPreviewBeatmapLevel::get_songDuration);
    DECLARE_OVERRIDE_METHOD_MATCH(::System::Threading::Tasks::Task_1<::UnityEngine::Sprite*>*, GetCoverImageAsync, &::GlobalNamespace::IPreviewBeatmapLevel::GetCoverImageAsync, ::System::Threading::CancellationToken cancellationToken);

    DECLARE_CTOR(ctor_1, StringW hash);
    public:
        static BeatSaverBeatmapLevel* Make(const std::string& hash, const BeatSaver::Beatmap& beatmap);
    protected:
        BeatSaver::Beatmap beatmap;
)
