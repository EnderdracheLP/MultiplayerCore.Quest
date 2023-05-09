#include "Beatmaps/LocalBeatmapLevel.hpp"
#include "Utils/ExtraSongData.hpp"

DEFINE_TYPE(MultiplayerCore::Beatmaps, LocalBeatmapLevel);

using namespace MultiplayerCore::Utils;

namespace MultiplayerCore::Beatmaps {

    void LocalBeatmapLevel::ctor_2(StringW hash, GlobalNamespace::IPreviewBeatmapLevel* preview) {
        INVOKE_CTOR();
        INVOKE_BASE_CTOR(classof(Abstractions::MpBeatmapLevel*));

        set_levelHash(hash);
        _preview = preview;

        auto extraSongData = ExtraSongData::FromPreviewBeatmapLevel(preview);
        if (extraSongData.has_value()) {
            auto& difficulties = extraSongData->difficulties;
            for (const auto& difficulty : difficulties) {
                if (difficulty.additionalDifficultyData) {
                    auto& list = requirements[difficulty.beatmapCharacteristicName][difficulty.difficulty];
                    for (const auto& req : difficulty.additionalDifficultyData->requirements)
                        list.emplace_back(req);
                }

                difficultyColors[difficulty.beatmapCharacteristicName][difficulty.difficulty] =
                    Abstractions::DifficultyColors(
                        difficulty.colorLeft, difficulty.colorRight,
                        difficulty.envColorLeft, difficulty.envColorRight,
                        difficulty.envColorLeftBoost, difficulty.envColorRightBoost,
                        difficulty.obstacleColor
                    );
            }

            for (const auto& contributor : extraSongData->contributors)
                contributors.emplace_back(contributor);
        }
    }

	StringW LocalBeatmapLevel::get_levelID() { return _preview->get_levelID();}
	StringW LocalBeatmapLevel::get_songName() { return _preview->get_songName();}
	StringW LocalBeatmapLevel::get_songSubName() { return _preview->get_songSubName();}
	StringW LocalBeatmapLevel::get_songAuthorName() { return _preview->get_songAuthorName();}
	StringW LocalBeatmapLevel::get_levelAuthorName() { return _preview->get_levelAuthorName();}
	float LocalBeatmapLevel::get_beatsPerMinute() { return _preview->get_beatsPerMinute();}
	float LocalBeatmapLevel::get_songDuration() { return _preview->get_songDuration();}
	::System::Collections::Generic::IReadOnlyList_1<::GlobalNamespace::PreviewDifficultyBeatmapSet*>* LocalBeatmapLevel::get_previewDifficultyBeatmapSets()	{ return _preview->get_previewDifficultyBeatmapSets();}
}
