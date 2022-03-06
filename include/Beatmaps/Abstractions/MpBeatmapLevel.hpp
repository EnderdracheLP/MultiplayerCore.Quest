#pragma once
#include "main.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"

___DECLARE_TYPE_WRAPPER_INHERITANCE(MultiplayerCore::Beatmaps::Abstractions, MpBeatmapLevel, Il2CppTypeEnum::IL2CPP_TYPE_CLASS,
	Il2CppObject, "MultiplayerCore.Beatmaps.Abstractions",
	classof(GlobalNamespace::IPreviewBeatmapLevel*), 0, nullptr,
	DECLARE_CTOR(New);

    // Basic Song Info/Metadata

    /// <summary>
    /// The local ID of the level. Can vary between clients.
    /// </summary>
    DECLARE_INSTANCE_FIELD(Il2CppString*, levelId);

    /// <summary>
    /// The hash of the level. Should be the same on all clients.
    /// </summary>
    DECLARE_INSTANCE_FIELD(Il2CppString*, levelHash);

    DECLARE_INSTANCE_FIELD(Il2CppString*, songName);
    DECLARE_INSTANCE_FIELD(Il2CppString*, songSubName);
    DECLARE_INSTANCE_FIELD(Il2CppString*, songAuthorName);
    DECLARE_INSTANCE_FIELD(Il2CppString*, levelAuthorName);

    DECLARE_INSTANCE_FIELD(float, beatsPerMinute);
    DECLARE_INSTANCE_FIELD(float, songDuration);
	DECLARE_INSTANCE_FIELD(float, previewStartTime);
	DECLARE_INSTANCE_FIELD(float, previewDuration);
	DECLARE_INSTANCE_FIELD(Array<GlobalNamespace::PreviewDifficultyBeatmapSet*>*, previewDifficultyBeatmapSets);

	//DECLARE_INSTANCE_FIELD(float, songTimeOffset); // Not needed
	//DECLARE_INSTANCE_FIELD(float, shuffle); // Not needed
	//DECLARE_INSTANCE_FIELD(float, shufflePeriod); // Not needed
	//DECLARE_INSTANCE_FIELD_DEFAULT(GlobalNamespace::EnvironmentInfoSO*, environmentInfo, nullptr); // Not needed, used for level load
	//DECLARE_INSTANCE_FIELD_DEFAULT(GlobalNamespace::EnvironmentInfoSO*, allDirectionsEnvironmentInfo, nullptr); // Not needed, used for level load

	DECLARE_OVERRIDE_METHOD(Il2CppString*, get_levelID, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_levelID", 0));
	DECLARE_OVERRIDE_METHOD(Il2CppString*, get_songName, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_songName", 0));
	DECLARE_OVERRIDE_METHOD(Il2CppString*, get_songSubName, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_songSubName", 0));
	DECLARE_OVERRIDE_METHOD(Il2CppString*, get_songAuthorName, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_songAuthorName", 0));
	DECLARE_OVERRIDE_METHOD(Il2CppString*, get_levelAuthorName, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_levelAuthorName", 0));
	DECLARE_OVERRIDE_METHOD(float, get_beatsPerMinute, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_beatsPerMinute", 0));
	DECLARE_OVERRIDE_METHOD(float, get_songDuration, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_songDuration", 0));
	DECLARE_OVERRIDE_METHOD(float, get_songTimeOffset, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_songTimeOffset", 0));
	DECLARE_OVERRIDE_METHOD(float, get_previewDuration, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_previewDuration", 0));
	DECLARE_OVERRIDE_METHOD(float, get_previewStartTime, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_previewStartTime", 0));
	DECLARE_OVERRIDE_METHOD(float, get_shuffle, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_shuffle", 0));
	DECLARE_OVERRIDE_METHOD(float, get_shufflePeriod, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_shufflePeriod", 0));
	DECLARE_OVERRIDE_METHOD(GlobalNamespace::EnvironmentInfoSO*, get_allDirectionsEnvironmentInfo, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_allDirectionsEnvironmentInfo", 0));
	DECLARE_OVERRIDE_METHOD(GlobalNamespace::EnvironmentInfoSO*, get_environmentInfo, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_environmentInfo", 0));
	DECLARE_OVERRIDE_METHOD(Array<GlobalNamespace::PreviewDifficultyBeatmapSet*>*, get_previewDifficultyBeatmapSets, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "get_previewDifficultyBeatmapSets", 0));

	DECLARE_OVERRIDE_METHOD(System::Threading::Tasks::Task_1<UnityEngine::Sprite*>*, GetCoverImageAsync, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::IPreviewBeatmapLevel*), "GetCoverImageAsync", 1), System::Threading::CancellationToken cancellationToken);
	)