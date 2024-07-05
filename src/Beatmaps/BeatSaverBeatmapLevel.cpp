#include "Beatmaps/BeatSaverBeatmapLevel.hpp"
#include "Beatmaps/BeatSaverPreviewMediaData.hpp"
#include "Utils/ExtraSongData.hpp"
#include "tasks.hpp"

#include "lapiz/shared/utilities/MainThreadScheduler.hpp"
#include "bsml/shared/Helpers/utilities.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"
#include "logging.hpp"

DEFINE_TYPE(MultiplayerCore::Beatmaps, BeatSaverBeatmapLevel);

using namespace MultiplayerCore::Utils;

namespace MultiplayerCore::Beatmaps {

    int32_t ConvertDifficulty(std::string difficulty)
    {
        if (difficulty == "Easy") return 0;
        if (difficulty == "Normal") return 1;
        if (difficulty == "Hard") return 2;
        if (difficulty == "Expert") return 3;
        if (difficulty == "ExpertPlus") return 4;
        return -1;
    }

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

        // TODO: Check if we need requirements and other extra data here
        auto v = std::find_if(beatmap.GetVersions().begin(), beatmap.GetVersions().end(), 
            [hash = std::string_view(hash)](auto& x){ return std::ranges::equal(x.GetHash(), hash, 
                [](char a, char b){
                    return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b)); 
                });
            }
        );
        if (v != beatmap.GetVersions().end()) {
            for (const auto& difficulty : v->GetDiffs()) {
                auto& list = level->requirements[difficulty.GetCharacteristic()][ConvertDifficulty(difficulty.GetDifficulty())];
                if (difficulty.GetChroma()) list.emplace_back("Chroma");
                if (difficulty.GetNE()) list.emplace_back("Noodle Extensions");
                if (difficulty.GetME()) list.emplace_back("Mapping Extensions");
            }
        }
        else {
            WARNING("Could not find version for hash {}", hash);
        }

        level->contributors.emplace_back(ExtraSongData::Contributor{ beatmap.GetMetadata().GetLevelAuthorName(), "Level Author", "" });
        level->contributors.emplace_back(ExtraSongData::Contributor{ beatmap.GetUploader().GetUsername(), "Uploader", beatmap.GetUploader().GetAvatarURL() });

        return level;
    }
}
