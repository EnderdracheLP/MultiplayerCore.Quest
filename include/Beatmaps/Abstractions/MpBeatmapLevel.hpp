#pragma once
#include "main.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/PreviewDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/EnvironmentInfoSO.hpp"

#include "UnityEngine/Sprite.hpp"

#include "System/Threading/Tasks/Task_1.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES_DLL(MultiplayerCore::Beatmaps::Abstractions, MpBeatmapLevel,
    Il2CppObject,
    { classof(GlobalNamespace::IPreviewBeatmapLevel*) },
    "MultiplayerCore.Beatmaps.Abstractions",

    DECLARE_CTOR(New);

    DECLARE_INSTANCE_METHOD(StringW, get_levelHash);

    DECLARE_OVERRIDE_METHOD(StringW, get_levelID, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_levelID>::get());

    DECLARE_OVERRIDE_METHOD(StringW, get_songName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songName>::get());
    DECLARE_OVERRIDE_METHOD(StringW, get_songSubName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songSubName>::get());
    DECLARE_OVERRIDE_METHOD(StringW, get_songAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songAuthorName>::get());
    DECLARE_OVERRIDE_METHOD(StringW, get_levelAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_levelAuthorName>::get());

    DECLARE_OVERRIDE_METHOD(float, get_beatsPerMinute, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_beatsPerMinute>::get());
    DECLARE_OVERRIDE_METHOD(float, get_songDuration, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songDuration>::get());
    DECLARE_OVERRIDE_METHOD(float, get_previewStartTime, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_previewStartTime>::get());
    DECLARE_OVERRIDE_METHOD(float, get_previewDuration, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_previewDuration>::get());
    DECLARE_OVERRIDE_METHOD(ArrayW<GlobalNamespace::PreviewDifficultyBeatmapSet*>, get_previewDifficultyBeatmapSets, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_previewDifficultyBeatmapSets>::get());

    DECLARE_OVERRIDE_METHOD(float, get_songTimeOffset, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songTimeOffset>::get());
    DECLARE_OVERRIDE_METHOD(float, get_shuffle, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_shuffle>::get());
    DECLARE_OVERRIDE_METHOD(float, get_shufflePeriod, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_shufflePeriod>::get());
    DECLARE_OVERRIDE_METHOD(GlobalNamespace::EnvironmentInfoSO*, get_environmentInfo, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_environmentInfo>::get());
    DECLARE_OVERRIDE_METHOD(GlobalNamespace::EnvironmentInfoSO*, get_allDirectionsEnvironmentInfo, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_allDirectionsEnvironmentInfo>::get());

    DECLARE_OVERRIDE_METHOD(System::Threading::Tasks::Task_1<UnityEngine::Sprite*>*, GetCoverImageAsync, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::GetCoverImageAsync>::get(), System::Threading::CancellationToken cancellationToken);
)