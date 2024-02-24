#include "Beatmaps/Abstractions/MpBeatmapLevel.hpp"
#include "logging.hpp"

DEFINE_TYPE(MultiplayerCore::Beatmaps::Abstractions, MpBeatmapLevel);

namespace MultiplayerCore::Beatmaps::Abstractions {
    void MpBeatmapLevel::ctor() { INVOKE_CTOR(); }
    ::StringW MpBeatmapLevel::get_levelID() { return fmt::format("custom_level_{}", get_levelHash()); }
    ::StringW MpBeatmapLevel::get_levelHash() { return levelHash; }
    ::StringW MpBeatmapLevel::get_songName() { return ""; }
    ::StringW MpBeatmapLevel::get_songSubName() { return ""; }
    ::StringW MpBeatmapLevel::get_songAuthorName() { return ""; }
    ::StringW MpBeatmapLevel::get_levelAuthorName() { return ""; }
    float MpBeatmapLevel::get_beatsPerMinute() { return 0; }
    float MpBeatmapLevel::get_songDuration() { return 0; }
    float MpBeatmapLevel::get_previewStartTime() { return 0; }
    float MpBeatmapLevel::get_previewDuration() { return 0;}
    ::System::Collections::Generic::IReadOnlyList_1<::GlobalNamespace::PreviewDifficultyBeatmapSet*>* MpBeatmapLevel::get_previewDifficultyBeatmapSets() { return nullptr; }
    float MpBeatmapLevel::get_songTimeOffset() { return 0; }
    float MpBeatmapLevel::get_shuffle() { return 0; }
    float MpBeatmapLevel::get_shufflePeriod() { return 0; }
    ::GlobalNamespace::EnvironmentInfoSO* MpBeatmapLevel::get_environmentInfo() { return nullptr; }
    ::GlobalNamespace::EnvironmentInfoSO* MpBeatmapLevel::get_allDirectionsEnvironmentInfo() { return nullptr; }
    ::ArrayW<::GlobalNamespace::EnvironmentInfoSO*> MpBeatmapLevel::get_environmentInfos() { return ArrayW<::GlobalNamespace::EnvironmentInfoSO*>::Empty(); };
    ::GlobalNamespace::PlayerSensitivityFlag MpBeatmapLevel::get_contentRating() { return ::GlobalNamespace::PlayerSensitivityFlag::Unknown; }

    ::System::Threading::Tasks::Task_1<::UnityEngine::Sprite*>* MpBeatmapLevel::GetCoverImageAsync(::System::Threading::CancellationToken cancellationToken) {
        return ::System::Threading::Tasks::Task_1<::UnityEngine::Sprite*>::FromResult<UnityEngine::Sprite*>(nullptr);
    }

    void MpBeatmapLevel::set_levelHash(StringW value) {
        levelHash = value;
    }
}
