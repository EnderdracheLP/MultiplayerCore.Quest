#pragma once

#include "Abstractions/MpBeatmapLevel.hpp"

DECLARE_CLASS_CUSTOM(MultiplayerCore::Beatmaps, LocalBeatmapLevel, Abstractions::MpBeatmapLevel,
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::IPreviewBeatmapLevel*, _preview);
    DECLARE_OVERRIDE_METHOD(StringW, get_levelID, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_levelID>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songName>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songSubName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songSubName>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songAuthorName>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_levelAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_levelAuthorName>::get());
	DECLARE_OVERRIDE_METHOD(float, get_beatsPerMinute, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_beatsPerMinute>::get());
	DECLARE_OVERRIDE_METHOD(float, get_songDuration, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songDuration>::get());
	DECLARE_OVERRIDE_METHOD(::System::Collections::Generic::IReadOnlyList_1<::GlobalNamespace::PreviewDifficultyBeatmapSet*>*, get_previewDifficultyBeatmapSets, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_previewDifficultyBeatmapSets>::get());

    DECLARE_CTOR(ctor_2, StringW hash, GlobalNamespace::IPreviewBeatmapLevel* preview);
)
