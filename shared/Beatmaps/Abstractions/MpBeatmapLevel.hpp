#pragma once

#include "custom-types/shared/macros.hpp"
#include "DifficultyColors.hpp"
#include "../../Utils/ExtraSongData.hpp"

#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "UnityEngine/Sprite.hpp"
#include "System/Collections/Generic/IReadOnlyList_1.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Threading/CancellationToken.hpp"

DECLARE_CLASS_CODEGEN(MultiplayerCore::Beatmaps::Abstractions, MpBeatmapLevel, GlobalNamespace::BeatmapLevel,
    DECLARE_INSTANCE_FIELD_PRIVATE(StringW, levelHash);
    public:
        std::string_view get_levelHash() const { return _levelHash; }

    DECLARE_CTOR(ctor);
    public:
        std::unordered_map<std::string, std::unordered_map<uint32_t, std::list<std::string>>> requirements;
        std::unordered_map<std::string, std::unordered_map<uint32_t, DifficultyColors>> difficultyColors;
        std::vector<const MultiplayerCore::Utils::ExtraSongData::Contributor> contributors;
    protected:
        std::string _levelHash;
        void set_levelHash(std::string_view levelHash) { _levelHash = levelHash; }
)
