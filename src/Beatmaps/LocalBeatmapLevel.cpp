#include "Beatmaps/LocalBeatmapLevel.hpp"
#include "custom-types/shared/register.hpp"
#include "Utils/ExtraSongData.hpp"
using namespace MultiplayerCore::Utils;

#include "GlobalNamespace/VarIntExtensions.hpp"
#include "songloader/shared/API.hpp"

DEFINE_TYPE(MultiplayerCore::Beatmaps, LocalBeatmapLevel);

namespace MultiplayerCore::Beatmaps {
	void LocalBeatmapLevel::New(StringW hash, GlobalNamespace::IPreviewBeatmapLevel* preview) {
		INVOKE_CTOR();
		levelHash = hash;
		std::optional<ExtraSongData> extraSongData = ExtraSongData::FromPreviewBeatmapLevel(preview);
		if (extraSongData)
		{
            auto difficulties = extraSongData->_difficulties;
			for (const Utils::ExtraSongData::DifficultyData& difficulty : difficulties)
			{
				if (!requirements.contains(difficulty._beatmapCharacteristicName))
				{
					std::unordered_map<uint8_t, std::vector<std::string>> reqsMap;
					reqsMap[difficulty._difficulty] = difficulty.additionalDifficultyData ? difficulty.additionalDifficultyData->_requirements : std::vector<std::string>();
					requirements[difficulty._beatmapCharacteristicName] = reqsMap;
				}

				if (!difficultyColors.contains(difficulty._beatmapCharacteristicName))
				{
					std::unordered_map<uint8_t, MultiplayerCore::Beatmaps::Abstractions::DifficultyColors> colorMap;
					colorMap[(uint8_t)difficulty._difficulty] = Abstractions::DifficultyColors(difficulty);
					difficultyColors[difficulty._beatmapCharacteristicName] = colorMap;
				}

				if (!difficultyColors.contains(difficulty._beatmapCharacteristicName))
				{
					std::unordered_map<uint8_t, MultiplayerCore::Beatmaps::Abstractions::DifficultyColors> colorMap;
					colorMap[(uint8_t)difficulty._difficulty] = Abstractions::DifficultyColors(difficulty);
					difficultyColors[difficulty._beatmapCharacteristicName] = colorMap;
				}

				contributors = std::move(extraSongData->contributors);
			}
		}
		_preview = preview;
	}

#pragma region All the pain in form of getter functions

	StringW LocalBeatmapLevel::get_levelID() {
		return _preview->get_levelID();
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
