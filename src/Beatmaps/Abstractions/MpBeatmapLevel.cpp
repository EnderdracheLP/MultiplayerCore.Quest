#include "Beatmaps/Abstractions/MpBeatmapLevel.hpp"
#include "logging.hpp"

DEFINE_TYPE(MultiplayerCore::Beatmaps::Abstractions, MpBeatmapLevel);

namespace MultiplayerCore::Beatmaps::Abstractions {
    void MpBeatmapLevel::ctor() { INVOKE_CTOR(); }
}
