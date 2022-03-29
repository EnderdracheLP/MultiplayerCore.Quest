#include "Beatmaps/LocalBeatmapLevel.hpp"
#include "custom-types/shared/register.hpp"
#include "GlobalNamespace/VarIntExtensions.hpp"
#include "songloader/shared/API.hpp"

DEFINE_TYPE(MultiplayerCore::Beatmaps, LocalBeatmapLevel);

namespace MultiplayerCore::Beatmaps {
	void LocalBeatmapLevel::New(StringW hash, GlobalNamespace::IPreviewBeatmapLevel* preview) {
		levelHash = hash;
		_preview = preview;
	}

#pragma region All the pain in form of getter functions

	StringW LocalBeatmapLevel::get_levelID() {
		return _preview->get_levelID();
	}

	StringW LocalBeatmapLevel::get_levelHash() {
		return levelHash;
	}

	StringW LocalBeatmapLevel::get_songName() {
		return _preview->get_songName();
	}

	StringW LocalBeatmapLevel::get_songSubName() {
		return _preview->get_songSubName();
	}

	StringW LocalBeatmapLevel::get_songAuthorName() {
		return _preview->get_songAuthorName();
	}

	StringW LocalBeatmapLevel::get_levelAuthorName() {
		return _preview->get_levelAuthorName();
	}

	float LocalBeatmapLevel::get_beatsPerMinute() {
		return _preview->get_beatsPerMinute();
	}

	float LocalBeatmapLevel::get_songDuration() {
		return _preview->get_songDuration();
	}

	::System::Collections::Generic::IReadOnlyList_1<::GlobalNamespace::PreviewDifficultyBeatmapSet*>* LocalBeatmapLevel::get_previewDifficultyBeatmapSets()	{
		return _preview->get_previewDifficultyBeatmapSets();
	}
#pragma endregion
}
