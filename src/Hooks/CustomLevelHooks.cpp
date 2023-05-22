#include "logging.hpp"
#include "hooking.hpp"
#include "Hooks/NetworkConfigHooks.hpp"
#include "Utilities.hpp"

#include "Beatmaps/NoInfoBeatmapLevel.hpp"

#include "UnityEngine/UI/Button.hpp"
#include "GlobalNamespace/CannotStartGameReason.hpp"
#include "GlobalNamespace/PreviewDifficultyBeatmap.hpp"
#include "GlobalNamespace/MultiplayerLevelSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/LevelSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/LevelSelectionNavigationController.hpp"
#include "GlobalNamespace/LobbySetupViewController.hpp"
#include "GlobalNamespace/BeatmapIdentifierNetSerializableHelper.hpp"
#include "GlobalNamespace/JoinQuickPlayViewController.hpp"
#include "GlobalNamespace/BeatmapDifficultyDropdown.hpp"
#include "GlobalNamespace/QuickPlaySetupModel.hpp"
#include "GlobalNamespace/BeatmapIdentifierNetSerializable.hpp"

using namespace MultiplayerCore;
using namespace MultiplayerCore::Hooks;

MAKE_AUTO_HOOK_MATCH(LobbySetupViewController_SetPlayersMissingLevelText, &::GlobalNamespace::LobbySetupViewController::SetPlayersMissingLevelText, void, GlobalNamespace::LobbySetupViewController* self, StringW playersMissingLevelText) {
    if (!Il2CppString::IsNullOrEmpty(playersMissingLevelText) && self->startGameReadyButton->get_interactable()) {
        self->SetStartGameEnabled(GlobalNamespace::CannotStartGameReason::DoNotOwnSong);
    }
    LobbySetupViewController_SetPlayersMissingLevelText(self, playersMissingLevelText);
}

MAKE_AUTO_HOOK_MATCH(BeatmapIdentifierNetSerializableHelper_ToPreviewDifficultyBeatmap, &::GlobalNamespace::BeatmapIdentifierNetSerializableHelper::ToPreviewDifficultyBeatmap, ::GlobalNamespace::PreviewDifficultyBeatmap*, ::GlobalNamespace::BeatmapIdentifierNetSerializable* beatmapId, ::GlobalNamespace::BeatmapLevelsModel* beatmapLevelsModel, ::GlobalNamespace::BeatmapCharacteristicCollectionSO* beatmapCharacteristicCollection) {
    auto res = BeatmapIdentifierNetSerializableHelper_ToPreviewDifficultyBeatmap(beatmapId, beatmapLevelsModel, beatmapCharacteristicCollection);
    if (!res->get_beatmapLevel())
        res->set_beatmapLevel(
            Beatmaps::NoInfoBeatmapLevel::New_ctor(
                Utilities::HashForLevelId(beatmapId->get_levelID())
            )->i_IPreviewBeatmapLevel()
        );
    return res;
}

MAKE_AUTO_HOOK_MATCH(JoinQuickPlayViewController_Setup, &::GlobalNamespace::JoinQuickPlayViewController::Setup, void, ::GlobalNamespace::JoinQuickPlayViewController* self, GlobalNamespace::QuickPlaySetupData *quickPlaySetupData, GlobalNamespace::MultiplayerModeSettings *multiplayerModeSettings) {
    if (NetworkConfigHooks::IsOverridingAPI()) self->beatmapDifficultyDropdown->set_includeAllDifficulties(true);
    JoinQuickPlayViewController_Setup(self, quickPlaySetupData, multiplayerModeSettings);
}

MAKE_AUTO_HOOK_MATCH(BeatmapDifficultyDropdown_GetIdxForBeatmapDifficultyMask, &::GlobalNamespace::BeatmapDifficultyDropdown::GetIdxForBeatmapDifficultyMask, int, ::GlobalNamespace::BeatmapDifficultyDropdown* self, GlobalNamespace::BeatmapDifficultyMask beatmapDifficultyMask) {
    auto res = BeatmapDifficultyDropdown_GetIdxForBeatmapDifficultyMask(self, beatmapDifficultyMask);
    if (self->includeAllDifficulties) return 0;
    return res;
}

MAKE_AUTO_HOOK_MATCH(QuickPlaySetupModel_IsQuickPlaySetupTaskValid, &::GlobalNamespace::QuickPlaySetupModel::IsQuickPlaySetupTaskValid, bool, ::GlobalNamespace::QuickPlaySetupModel* self) {
    auto res = QuickPlaySetupModel_IsQuickPlaySetupTaskValid(self);
    if (NetworkConfigHooks::IsOverridingAPI()) return false;
    return res;
}

MAKE_AUTO_HOOK_MATCH(LevelSelectionNavigationController_Setup, &::GlobalNamespace::LevelSelectionNavigationController::Setup, void,
    GlobalNamespace::LevelSelectionNavigationController* self,
    GlobalNamespace::SongPackMask songPackMask,
    GlobalNamespace::BeatmapDifficultyMask allowedBeatmapDifficultyMask,
    ::ArrayW<GlobalNamespace::BeatmapCharacteristicSO*> notAllowedCharacteristics,
    bool hidePacksIfOneOrNone,
    bool hidePracticeButton,
    ::StringW actionButtonText,
    GlobalNamespace::IBeatmapLevelPack* levelPackToBeSelectedAfterPresent,
    GlobalNamespace::SelectLevelCategoryViewController::LevelCategory startLevelCategory,
    GlobalNamespace::IPreviewBeatmapLevel* beatmapLevelToBeSelectedAfterPresent,
    bool enableCustomLevels
) {
    ConstString custom_levelpack_CustomLevels("custom_levelpack_CustomLevels");
    LevelSelectionNavigationController_Setup(
        self,
        songPackMask,
        allowedBeatmapDifficultyMask,
        notAllowedCharacteristics,
        hidePacksIfOneOrNone,
        hidePracticeButton,
        actionButtonText,
        levelPackToBeSelectedAfterPresent,
        startLevelCategory,
        beatmapLevelToBeSelectedAfterPresent,
        enableCustomLevels || songPackMask.Contains(StringW(custom_levelpack_CustomLevels))
    );
}
