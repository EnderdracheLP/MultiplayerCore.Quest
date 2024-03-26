#include "Beatmaps/BeatSaverBeatmapLevel.hpp"
#include "Beatmaps/BeatSaverPreviewMediaData.hpp"
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

        set_levelHash(static_cast<std::string>(hash));
    }

    BeatSaverBeatmapLevel* BeatSaverBeatmapLevel::Make(const std::string& hash, const BeatSaver::Beatmap& beatmap) {
        auto level = BeatSaverBeatmapLevel::New_ctor(hash);
        level->beatmap = beatmap;

        level->_ctor(
            false,
            fmt::format("custom_level_{}", hash),
            beatmap.GetMetadata().GetSongName(),
            beatmap.GetMetadata().GetSongSubName(),
            beatmap.GetMetadata().GetSongAuthorName(),
            ArrayW<StringW>({ StringW(beatmap.GetMetadata().GetLevelAuthorName()) }),
            ArrayW<StringW>::Empty(),
            beatmap.GetMetadata().GetBPM(),
            -6.0f,
            0,
            0,
            0,
            beatmap.GetMetadata().GetDuration(),
            GlobalNamespace::PlayerSensitivityFlag::Unknown,
            BeatSaverPreviewMediaData::New_ctor(hash)->i_IPreviewMediaData(),
            nullptr
        );

        return level;
    }
}
