#pragma once
#include <string>
#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"

namespace MultiplayerCore::Utils {
    bool HasRequirement(const std::optional<GlobalNamespace::CustomPreviewBeatmapLevel*>& beatmapOpt);
}
