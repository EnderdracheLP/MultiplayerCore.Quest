#include "Beatmaps/Abstractions/MpBeatmapLevel.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "System/Threading/Tasks/TaskStatus.hpp"
#include "UnityEngine/ImageConversion.hpp"
#include "GlobalNamespace/MediaAsyncLoader.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include <vector>

DEFINE_TYPE(MultiplayerCore::Beatmaps::Abstractions, MpBeatmapLevel);

namespace MultiplayerCore::Beatmaps::Abstractions {

	System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* MpBeatmapLevel::GetCoverImageAsync(System::Threading::CancellationToken cancellationToken) {
		return System::Threading::Tasks::Task_1<UnityEngine::Sprite*>::New_ctor(static_cast<UnityEngine::Sprite*>(nullptr));
	}

#pragma region All the pain in form of getter functions

	Il2CppString* MpBeatmapLevel::get_levelID() {
		return levelID;
	}

	Il2CppString* MpBeatmapLevel::get_songName() {
		return songName;
	}

	Il2CppString* MpBeatmapLevel::get_songSubName() {
		return songSubName;
	}

	Il2CppString* MpBeatmapLevel::get_songAuthorName() {
		return songAuthorName;
	}

	Il2CppString* MpBeatmapLevel::get_levelAuthorName() {
		return levelAuthorName;
	}

	float MpBeatmapLevel::get_beatsPerMinute() {
		return beatsPerMinute;
	}

	float MpBeatmapLevel::get_songDuration() {
		return songDuration;
	}

	float MpBeatmapLevel::get_songTimeOffset() {
		return songTimeOffset;
	}

	float MpBeatmapLevel::get_previewDuration() {
		return previewDuration;
	}

	float MpBeatmapLevel::get_previewStartTime() {
		return previewStartTime;
	}

	float MpBeatmapLevel::get_shuffle() {
		return shuffle;
	}

	float MpBeatmapLevel::get_shufflePeriod() {
		return shufflePeriod;
	}

	GlobalNamespace::EnvironmentInfoSO* MpBeatmapLevel::get_allDirectionsEnvironmentInfo() {
		return nullptr;
	}

	GlobalNamespace::EnvironmentInfoSO* MpBeatmapLevel::get_environmentInfo() {
		return nullptr;
	}

	Array<GlobalNamespace::PreviewDifficultyBeatmapSet*>* MpBeatmapLevel::get_previewDifficultyBeatmapSets() {
		return previewDifficultyBeatmapSets;
	}

#pragma endregion

}