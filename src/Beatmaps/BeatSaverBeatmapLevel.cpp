#include "Beatmaps/BeatSaverBeatmapLevel.hpp"
#include "Utils/ExtraSongData.hpp"

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
}
