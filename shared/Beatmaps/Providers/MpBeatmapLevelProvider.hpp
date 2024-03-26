#pragma once

#include "custom-types/shared/macros.hpp"
#include "System/Threading/Tasks/Task.hpp"
#include "System/Threading/Tasks/Task_1.hpp"

#include "GlobalNamespace/BeatmapLevel.hpp"
#include "../Packets/MpBeatmapPacket.hpp"
#include <future>

DECLARE_CLASS_CODEGEN(MultiplayerCore::Beatmaps::Providers, MpBeatmapLevelProvider, System::Object,
    std::future<GlobalNamespace::BeatmapLevel*> GetBeatmapAsync(const std::string& levelHash);
    std::future<GlobalNamespace::BeatmapLevel*> GetBeatmapFromBeatSaverAsync(const std::string& levelHash);

    GlobalNamespace::BeatmapLevel* GetBeatmapFromLocalBeatmaps(const std::string& levelHash);
    GlobalNamespace::BeatmapLevel* GetBeatmapFromPacket(Packets::MpBeatmapPacket* packet);

    DECLARE_DEFAULT_CTOR();
    protected:
        GlobalNamespace::BeatmapLevel* GetBeatmapFromBeatSaver(std::string levelHash);
)
