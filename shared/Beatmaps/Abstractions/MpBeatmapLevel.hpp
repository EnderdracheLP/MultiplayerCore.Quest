#pragma once

#include "custom-types/shared/macros.hpp"
#include "DifficultyColors.hpp"
#include "../../Utils/ExtraSongData.hpp"

#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "GlobalNamespace/PreviewDifficultyBeatmapSet.hpp"
#include "UnityEngine/Sprite.hpp"
#include "System/Collections/Generic/IReadOnlyList_1.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Threading/CancellationToken.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerCore::Beatmaps::Abstractions, MpBeatmapLevel, System::Object, classof(GlobalNamespace::IPreviewBeatmapLevel*),
    DECLARE_INSTANCE_FIELD_PRIVATE(StringW, levelHash);
    DECLARE_OVERRIDE_METHOD_MATCH(::StringW, get_levelID, &::GlobalNamespace::IPreviewBeatmapLevel::get_levelID);

    public:
        StringW get_levelHash();

    DECLARE_OVERRIDE_METHOD_MATCH(::StringW, get_songName, &::GlobalNamespace::IPreviewBeatmapLevel::get_songName);
    DECLARE_OVERRIDE_METHOD_MATCH(::StringW, get_songSubName, &::GlobalNamespace::IPreviewBeatmapLevel::get_songSubName);
    DECLARE_OVERRIDE_METHOD_MATCH(::StringW, get_songAuthorName, &::GlobalNamespace::IPreviewBeatmapLevel::get_songAuthorName);
    DECLARE_OVERRIDE_METHOD_MATCH(::StringW, get_levelAuthorName, &::GlobalNamespace::IPreviewBeatmapLevel::get_levelAuthorName);

    DECLARE_OVERRIDE_METHOD_MATCH(float, get_beatsPerMinute, &::GlobalNamespace::IPreviewBeatmapLevel::get_beatsPerMinute);
    DECLARE_OVERRIDE_METHOD_MATCH(float, get_songDuration, &::GlobalNamespace::IPreviewBeatmapLevel::get_songDuration);
    DECLARE_OVERRIDE_METHOD_MATCH(float, get_previewStartTime, &::GlobalNamespace::IPreviewBeatmapLevel::get_previewStartTime);
    DECLARE_OVERRIDE_METHOD_MATCH(float, get_previewDuration, &::GlobalNamespace::IPreviewBeatmapLevel::get_previewDuration);
    DECLARE_OVERRIDE_METHOD_MATCH(::System::Collections::Generic::IReadOnlyList_1<::GlobalNamespace::PreviewDifficultyBeatmapSet*>*, get_previewDifficultyBeatmapSets, &::GlobalNamespace::IPreviewBeatmapLevel::get_previewDifficultyBeatmapSets);

    DECLARE_OVERRIDE_METHOD_MATCH(float, get_songTimeOffset, &::GlobalNamespace::IPreviewBeatmapLevel::get_songTimeOffset);
    DECLARE_OVERRIDE_METHOD_MATCH(float, get_shuffle, &::GlobalNamespace::IPreviewBeatmapLevel::get_shuffle);
    DECLARE_OVERRIDE_METHOD_MATCH(float, get_shufflePeriod, &::GlobalNamespace::IPreviewBeatmapLevel::get_shufflePeriod);
    DECLARE_OVERRIDE_METHOD_MATCH(::GlobalNamespace::EnvironmentInfoSO*, get_environmentInfo, &::GlobalNamespace::IPreviewBeatmapLevel::get_environmentInfo);
    DECLARE_OVERRIDE_METHOD_MATCH(::GlobalNamespace::EnvironmentInfoSO*, get_allDirectionsEnvironmentInfo, &::GlobalNamespace::IPreviewBeatmapLevel::get_allDirectionsEnvironmentInfo);
    DECLARE_OVERRIDE_METHOD_MATCH(::ArrayW<::GlobalNamespace::EnvironmentInfoSO*>, get_environmentInfos, &::GlobalNamespace::IPreviewBeatmapLevel::get_environmentInfos);
    DECLARE_OVERRIDE_METHOD_MATCH(::GlobalNamespace::PlayerSensitivityFlag, get_contentRating, &::GlobalNamespace::IPreviewBeatmapLevel::get_contentRating);

    DECLARE_OVERRIDE_METHOD_MATCH(::System::Threading::Tasks::Task_1<::UnityEngine::Sprite*>*, GetCoverImageAsync, &::GlobalNamespace::IPreviewBeatmapLevel::GetCoverImageAsync, ::System::Threading::CancellationToken cancellationToken);

    DECLARE_CTOR(ctor);
    public:
        std::unordered_map<std::string, std::unordered_map<uint8_t, std::list<std::string>>> requirements;
        std::unordered_map<std::string, std::unordered_map<uint8_t, DifficultyColors>> difficultyColors;
        std::vector<const MultiplayerCore::Utils::ExtraSongData::Contributor> contributors;

        GlobalNamespace::IPreviewBeatmapLevel* i_IPreviewBeatmapLevel() { return reinterpret_cast<GlobalNamespace::IPreviewBeatmapLevel*>(this); }
        operator GlobalNamespace::IPreviewBeatmapLevel* () { return reinterpret_cast<GlobalNamespace::IPreviewBeatmapLevel*>(this); }
    protected:
        void set_levelHash(StringW value);

)
