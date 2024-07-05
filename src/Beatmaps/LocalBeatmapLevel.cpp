#include "Beatmaps/LocalBeatmapLevel.hpp"
#include "Utils/ExtraSongData.hpp"
#include "logging.hpp"

DEFINE_TYPE(MultiplayerCore::Beatmaps, LocalBeatmapLevel);

using namespace MultiplayerCore::Utils;

namespace MultiplayerCore::Beatmaps {

    void LocalBeatmapLevel::ctor_2(StringW hash, GlobalNamespace::BeatmapLevel* localLevel) {
        INVOKE_CTOR();
        INVOKE_BASE_CTOR(classof(Abstractions::MpBeatmapLevel*));

        // invoke beatmaplevel ctor
        _ctor(
            localLevel->version,
            localLevel->hasPrecalculatedData,
            localLevel->levelID,
            localLevel->songName,
            localLevel->songSubName,
            localLevel->songAuthorName,
            localLevel->allMappers,
            localLevel->allLighters,
            localLevel->beatsPerMinute,
            localLevel->integratedLufs,
            localLevel->songTimeOffset,
            localLevel->previewStartTime,
            localLevel->previewDuration,
            localLevel->songDuration,
            localLevel->contentRating,
            localLevel->previewMediaData,
            localLevel->beatmapBasicData
        );

        _localLevel = localLevel;
        set_levelHash(static_cast<std::string>(hash));

        auto extraSongData = ExtraSongData::FromBeatmapLevel(localLevel);
        if (extraSongData.has_value()) {
            DEBUG("Got extra song data for level {}", hash);
            auto& difficulties = extraSongData->difficulties;
            for (const auto& difficulty : difficulties) {
                if (difficulty.additionalDifficultyData) {
                    auto& list = requirements[difficulty.beatmapCharacteristicName][difficulty.difficulty.value__];
                    for (const auto& req : difficulty.additionalDifficultyData->requirements)
                        list.emplace_back(req);
                }

                difficultyColors[difficulty.beatmapCharacteristicName][difficulty.difficulty.value__] =
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
        else {
            DEBUG("Failed to get extra song data for level {}", hash);
        }
    }
}
