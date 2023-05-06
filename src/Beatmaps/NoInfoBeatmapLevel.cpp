#include "Beatmaps/NoInfoBeatmapLevel.hpp"

DEFINE_TYPE(MultiplayerCore::Beatmaps, NoInfoBeatmapLevel);

namespace MultiplayerCore::Beatmaps {
    void NoInfoBeatmapLevel::ctor_1(StringW hash) {
        INVOKE_CTOR();
        INVOKE_BASE_CTOR(classof(Abstractions::MpBeatmapLevel*));

        set_levelHash(hash);
    }

    ConstString emptyString("");

    StringW NoInfoBeatmapLevel::get_songName() { return emptyString; }
    StringW NoInfoBeatmapLevel::get_songSubName() { return emptyString; }
    StringW NoInfoBeatmapLevel::get_songAuthorName() { return emptyString; }
    StringW NoInfoBeatmapLevel::get_levelAuthorName() { return emptyString; }
}
