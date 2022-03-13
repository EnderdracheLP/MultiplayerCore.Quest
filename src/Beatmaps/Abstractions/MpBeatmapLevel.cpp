#include "Beatmaps/Abstractions/MpBeatmapLevel.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Threading/Tasks/TaskStatus.hpp"

#include "UnityEngine/ImageConversion.hpp"

#include "GlobalNamespace/MediaAsyncLoader.hpp"

#include <vector>

DEFINE_TYPE(MultiplayerCore::Beatmaps::Abstractions, MpBeatmapLevel);

namespace MultiplayerCore::Beatmaps::Abstractions {

	void MpBeatmapLevel::New() {}

	System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* MpBeatmapLevel::GetCoverImageAsync(System::Threading::CancellationToken cancellationToken) {
		return System::Threading::Tasks::Task_1<UnityEngine::Sprite*>::New_ctor(static_cast<UnityEngine::Sprite*>(nullptr));
	}

#pragma region All the pain in form of getter functions

	StringW MpBeatmapLevel::get_levelHash() {
		return StringW();
	}


	StringW MpBeatmapLevel::get_levelID() {
		return StringW("custom_level_") + get_levelHash();
	}

	StringW MpBeatmapLevel::get_songName() {
		return StringW();
	}

	StringW MpBeatmapLevel::get_songSubName() {
		return StringW();
	}

	StringW MpBeatmapLevel::get_songAuthorName() {
		return StringW();
	}

	StringW MpBeatmapLevel::get_levelAuthorName() {
		return StringW();
	}

	float MpBeatmapLevel::get_beatsPerMinute() {
		return 0;
	}

	float MpBeatmapLevel::get_songDuration() {
		return 0;
	}

	float MpBeatmapLevel::get_songTimeOffset() {
		return 0;
	}

	float MpBeatmapLevel::get_previewDuration() {
		return 0;
	}

	float MpBeatmapLevel::get_previewStartTime() {
		return 0;
	}

	float MpBeatmapLevel::get_shuffle() {
		return 0;
	}

	float MpBeatmapLevel::get_shufflePeriod() {
		return 0;
	}

	GlobalNamespace::EnvironmentInfoSO* MpBeatmapLevel::get_allDirectionsEnvironmentInfo() {
		return nullptr;
	}

	GlobalNamespace::EnvironmentInfoSO* MpBeatmapLevel::get_environmentInfo() {
		return nullptr;
	}

	ArrayW<GlobalNamespace::PreviewDifficultyBeatmapSet*> MpBeatmapLevel::get_previewDifficultyBeatmapSets() {
		return ArrayW<GlobalNamespace::PreviewDifficultyBeatmapSet*>();
	}

#pragma endregion

}