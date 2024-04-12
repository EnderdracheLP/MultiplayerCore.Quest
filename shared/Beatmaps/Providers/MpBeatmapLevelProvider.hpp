#pragma once

#include "custom-types/shared/macros.hpp"
#include "System/Threading/Tasks/Task.hpp"
#include "System/Threading/Tasks/Task_1.hpp"

#include "GlobalNamespace/BeatmapLevel.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "../Packets/MpBeatmapPacket.hpp"
#include <future>

DECLARE_CLASS_CODEGEN(MultiplayerCore::Beatmaps::Providers, MpBeatmapLevelProvider, System::Object,
        using HashToLevelDict = System::Collections::Generic::Dictionary_2<StringW, GlobalNamespace::BeatmapLevel*>;
        DECLARE_INSTANCE_FIELD_PRIVATE(HashToLevelDict*, _hashToNetworkLevels);
        DECLARE_INSTANCE_FIELD_PRIVATE(HashToLevelDict*, _hashToBeatsaverLevels);
        DECLARE_CTOR(ctor);

    public:
        std::future<GlobalNamespace::BeatmapLevel*> GetBeatmapAsync(const std::string& levelHash);
        std::future<GlobalNamespace::BeatmapLevel*> GetBeatmapFromBeatSaverAsync(const std::string& levelHash);

        GlobalNamespace::BeatmapLevel* GetBeatmapFromLocalBeatmaps(const std::string& levelHash);
        GlobalNamespace::BeatmapLevel* GetBeatmapFromPacket(Packets::MpBeatmapPacket* packet);
        GlobalNamespace::BeatmapLevel* GetBeatmapFromBeatSaver(std::string levelHash);
)
