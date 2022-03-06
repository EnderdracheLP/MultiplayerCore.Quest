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

	//void MpBeatmapLevel::FromPreviewPacket(Il2CppString* levelhash, GlobalNamespace::IPreviewBeatmapLevel* level, PreviewBeatmapPacket* lvl) {
	//	if (levelhash && level) {
	//		getLogger().debug("PreviewBeatmapStub::FromPreview");

	//		_preview = level;
	//		isDownloaded = true;
	//		_downloadable = DownloadableState::Unchecked;

	//		levelID = level->get_levelID();
	//		levelHash = levelhash;
	//		//getLogger().debug("levelID: %s, levelHash: %s", to_utf8(csstrtostr(levelID)).c_str(), to_utf8(csstrtostr(levelHash)).c_str());

	//		songName = level->get_songName();
	//		songSubName = level->get_songSubName();
	//		songAuthorName = level->get_songAuthorName();
	//		levelAuthorName = level->get_levelAuthorName();
	//		//getLogger().debug("songName: %s, songSubName: %s, songAuthorName: %s, levelAuthorName: %s", to_utf8(csstrtostr(songName)).c_str(), to_utf8(csstrtostr(songSubName)).c_str(), to_utf8(csstrtostr(songAuthorName)).c_str(), to_utf8(csstrtostr(levelAuthorName)).c_str());

	//		beatsPerMinute = level->get_beatsPerMinute();
	//		songDuration = level->get_songDuration();
	//		//getLogger().debug("beatsPerMinute: %f, songDuration: %f", beatsPerMinute, songDuration);
	//	}
	//	else {
	//		getLogger().debug("PreviewBeatmapStub::FromPacket");

	//		//getLogger().debug("Set downloadstates");
	//		isDownloaded = false;
	//		_downloadable = DownloadableState::Unchecked;

	//		//getLogger().debug("Get levelId and Hash");
	//		levelID = lvl->levelId;
	//		levelHash = lvl->levelHash;
	//		//levelHash = LevelIdToHash(levelID)/*packet->levelHash*/;
	//		//getLogger().debug("levelID: %s\n Hash: %s",
	//		//	to_utf8(csstrtostr(levelID)).c_str(),
	//		//	to_utf8(csstrtostr(levelHash)).c_str());
	//		//getLogger().debug("SongInfo strings");
	//		songName = lvl->songName;
	//		songSubName = lvl->songSubName;
	//		songAuthorName = lvl->songAuthorName;
	//		levelAuthorName = lvl->levelAuthorName;
	//		//getLogger().debug("songName: %s\n songSubName: %s\n songAuthorName: %s\n levelAuthorName: %s",
	//		//	to_utf8(csstrtostr(songName)).c_str(),
	//		//	to_utf8(csstrtostr(songSubName)).c_str(),
	//		//	to_utf8(csstrtostr(songAuthorName)).c_str(),
	//		//	to_utf8(csstrtostr(levelAuthorName)).c_str());

	//		//getLogger().debug("SongInfo floats");
	//		beatsPerMinute = lvl->beatsPerMinute;
	//		songDuration = lvl->songDuration;
	//		//getLogger().debug("beatsPerMinute: %f\n songDuration: %f",
	//		//	beatsPerMinute,
	//		//	songDuration);
	//	}

	//}

	System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* MpBeatmapLevel::GetCoverImageAsync(System::Threading::CancellationToken cancellationToken) {
		//if (_preview) {
		//	getLogger().debug("Use localPreview");
		//	return _preview->GetCoverImageAsync(cancellationToken);
		//}
		//if (coverImage)
		//	return System::Threading::Tasks::Task_1<UnityEngine::Sprite*>::New_ctor(coverImage);
		//else {
			return System::Threading::Tasks::Task_1<UnityEngine::Sprite*>::New_ctor(static_cast<UnityEngine::Sprite*>(nullptr));
		//}
	}

	//System::Threading::Tasks::Task_1<UnityEngine::AudioClip*>* PreviewBeatmapStub::GetPreviewAudioClipAsync(System::Threading::CancellationToken cancellationToken) {
	//	if (_preview) {
	//		return _preview->GetPreviewAudioClipAsync(cancellationToken);
	//	}
	//	else {
	//		return nullptr;
	//	}
	//}

	//MultiplayerCore::Beatmaps::PreviewBeatmapPacket* MpBeatmapLevel::GetPacket(Il2CppString* characteristic, GlobalNamespace::BeatmapDifficulty difficulty) {
	//	getLogger().debug("Start PreviewBeatmapStub::GetPacket");
	//	MultiQuestensions::Beatmaps::PreviewBeatmapPacket* packet = THROW_UNLESS(il2cpp_utils::New<MultiQuestensions::Beatmaps::PreviewBeatmapPacket*>());

	//	packet->levelId = levelID;
	//	packet->levelHash = levelHash;

	//	packet->songName = songName;
	//	packet->songSubName = songSubName;
	//	packet->songAuthorName = songAuthorName;
	//	packet->levelAuthorName = levelAuthorName;

	//	packet->beatsPerMinute = beatsPerMinute;
	//	packet->songDuration = songDuration;

	//	packet->characteristic = characteristic;
	//	packet->difficulty = difficulty;
	//	return packet;
	//}

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