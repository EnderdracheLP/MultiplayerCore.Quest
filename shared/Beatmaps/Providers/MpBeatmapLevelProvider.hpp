#pragma once

#include "custom-types/shared/macros.hpp"
#include "System/Threading/Tasks/Task.hpp"
#include "System/Threading/Tasks/Task_1.hpp"

#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "../Packets/MpBeatmapPacket.hpp"
#include <future>

DECLARE_CLASS_CODEGEN(MultiplayerCore::Beatmaps::Providers, MpBeatmapLevelProvider, Il2CppObject,
    std::future<GlobalNamespace::IPreviewBeatmapLevel*> GetBeatmapAsync(const std::string& levelHash);
    std::future<GlobalNamespace::IPreviewBeatmapLevel*> GetBeatmapFromBeatSaverAsync(const std::string& levelHash);

    GlobalNamespace::IPreviewBeatmapLevel* GetBeatmapFromLocalBeatmaps(const std::string& levelHash);
    GlobalNamespace::IPreviewBeatmapLevel* GetBeatmapFromPacket(Packets::MpBeatmapPacket* packet);

    DECLARE_DEFAULT_CTOR();
    protected:
        GlobalNamespace::IPreviewBeatmapLevel* GetBeatmapFromBeatSaver(std::string levelHash);
)
