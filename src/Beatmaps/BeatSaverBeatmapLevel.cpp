#include "Beatmaps/BeatSaverBeatmapLevel.hpp"
#include "Utils/ExtraSongData.hpp"
#include "tasks.hpp"

#include "lapiz/shared/utilities/MainThreadScheduler.hpp"
#include "bsml/shared/Helpers/utilities.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"

DEFINE_TYPE(MultiplayerCore::Beatmaps, BeatSaverBeatmapLevel);

using namespace MultiplayerCore::Utils;

namespace MultiplayerCore::Beatmaps {

    void BeatSaverBeatmapLevel::ctor_1(StringW hash) {
        INVOKE_CTOR();
        INVOKE_BASE_CTOR(classof(Abstractions:: MpBeatmapLevel*));

        set_levelHash(hash);
    }

    BeatSaverBeatmapLevel* BeatSaverBeatmapLevel::Make(const std::string& hash, const BeatSaver::Beatmap& beatmap) {
        auto level = BeatSaverBeatmapLevel::New_ctor(hash);
        level->beatmap = beatmap;
        return level;
    }

	StringW BeatSaverBeatmapLevel::get_songName() { return beatmap.GetMetadata().GetSongName(); }
	StringW BeatSaverBeatmapLevel::get_songSubName() { return beatmap.GetMetadata().GetSongSubName(); }
	StringW BeatSaverBeatmapLevel::get_songAuthorName() { return beatmap.GetMetadata().GetSongAuthorName(); }
	StringW BeatSaverBeatmapLevel::get_levelAuthorName() { return beatmap.GetMetadata().GetLevelAuthorName(); }
	float BeatSaverBeatmapLevel::get_beatsPerMinute() { return beatmap.GetMetadata().GetBPM(); }
	float BeatSaverBeatmapLevel::get_songDuration() { return beatmap.GetMetadata().GetDuration(); }
    ::System::Threading::Tasks::Task_1<::UnityEngine::Sprite*>* BeatSaverBeatmapLevel::GetCoverImageAsync(::System::Threading::CancellationToken cancellationToken) {
        if (!_coverImageTask) {
            _coverImageTask = StartTask<UnityEngine::Sprite*>([this](){
                auto beatmapOpt = BeatSaver::API::GetBeatmapByHash(get_levelHash());
                if (beatmapOpt.has_value()) {
                    auto cover = BeatSaver::API::GetCoverImage(beatmapOpt.value());

                    auto coverBytes = il2cpp_utils::vectorToArray(cover);
                    std::optional<UnityEngine::Sprite*> result;
                    Lapiz::Utilities::MainThreadScheduler::Schedule([coverBytes, &result](){
                        result = BSML::Utilities::LoadSpriteRaw(coverBytes);
                    });

                    while(!result.has_value()) std::this_thread::yield();
                    return result.value();
                }
                return (UnityEngine::Sprite*)nullptr;
            }, cancellationToken);
        }
        return _coverImageTask;
    }
}
