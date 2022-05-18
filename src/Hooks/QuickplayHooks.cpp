#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "GlobalFields.hpp"

#include "GlobalNamespace/QuickPlaySetupData_QuickPlaySongPacksOverride_PredefinedPack.hpp"
#include "GlobalNamespace/QuickPlaySetupData_QuickPlaySongPacksOverride_LocalizedCustomPack.hpp"
#include "GlobalNamespace/QuickPlaySetupData_QuickPlaySongPacksOverride_LocalizedCustomPackName.hpp"
#include "GlobalNamespace/QuickPlaySetupData_QuickPlaySongPacksOverride.hpp"
#include "GlobalNamespace/QuickPlaySetupData.hpp"

#include "GlobalNamespace/BeatmapDifficultyDropdown.hpp"
#include "GlobalNamespace/BeatmapDifficultyMask.hpp"
#include "HMUI/SimpleTextDropdown.hpp"

#include "CodegenExtensions/TempBloomFilterUtil.hpp"

#include "GlobalNamespace/SongPackMaskModelSO.hpp"

#include "GlobalNamespace/MultiplayerModeSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/JoinQuickPlayViewController.hpp"
#include "GlobalNamespace/SimpleDialogPromptViewController.hpp"
#include "GlobalNamespace/MultiplayerModeSettings.hpp"

#include "HMUI/ViewController_AnimationDirection.hpp"
#include "HMUI/ViewController_AnimationType.hpp"

#include "Polyglot/Localization.hpp"
#include "Polyglot/LanguageExtensions.hpp"

#include "GlobalNamespace/QuickPlaySongPacksDropdown.hpp"

//Nothing here causes blackscreen crash

using namespace GlobalNamespace;
using namespace HMUI;

using MSQSD_QPSPO_PredefinedPack = QuickPlaySetupData::QuickPlaySongPacksOverride::PredefinedPack;
using MSQSD_QPSPO_LocalizedCustomPack = QuickPlaySetupData::QuickPlaySongPacksOverride::LocalizedCustomPack;
using MSQD_QPSPO_LocalizedCustomPackName = QuickPlaySetupData::QuickPlaySongPacksOverride::LocalizedCustomPackName;

bool gotSongPackOverrides = false;

namespace MultiplayerCore {
    // Check for our custom packs
    MAKE_HOOK_MATCH(QuickPlaySongPacksDropdown_LazyInit, &QuickPlaySongPacksDropdown::LazyInit, void, QuickPlaySongPacksDropdown* self) {
        gotSongPackOverrides = (self->dyn__quickPlaySongPacksOverride() != nullptr);
        QuickPlaySongPacksDropdown_LazyInit(self);
    }

    //Adds All dificulty to quickplay
    MAKE_HOOK_MATCH(JoinQuickPlayViewController_setup, &JoinQuickPlayViewController::Setup, void, JoinQuickPlayViewController* self, GlobalNamespace::QuickPlaySetupData* quickPlaySetupData, ::GlobalNamespace::MultiplayerModeSettings* multiplayerModeSettings){
        auto &Difficulties = self->dyn__beatmapDifficultyDropdown();
        Difficulties->set_includeAllDifficulties(true);
        
        JoinQuickPlayViewController_setup(self, quickPlaySetupData, multiplayerModeSettings);
        Difficulties->dyn__simpleTextDropdown()->SelectCellWithIdx(0);
    }

    //Adds warning screen about custom quickplay
    MAKE_HOOK_MATCH(MultiplayerModeSelectionFlowCoordinator_HandleJoinQuickPlayViewControllerDidFinish, &MultiplayerModeSelectionFlowCoordinator::HandleJoinQuickPlayViewControllerDidFinish, void, MultiplayerModeSelectionFlowCoordinator* self, bool success) {
        Il2CppString* levelPackName = self->dyn__joinQuickPlayViewController()->dyn__multiplayerModeSettings()->dyn_quickPlaySongPackMaskSerializedName();
        if (success && 
            self->dyn__songPackMaskModel()->ToSongPackMask(
                levelPackName
            ).Contains(
                getCustomLevelSongPackMaskStr())
            ) {
            self->dyn__simpleDialogPromptViewController()->Init(
                il2cpp_utils::newcsstr("Custom Song Quickplay"),
                il2cpp_utils::newcsstr("<color=#EB4949>This category includes songs of varying difficulty.\nIt may be more enjoyable to play in a private lobby with friends."),
                il2cpp_utils::newcsstr("Continue"),
                il2cpp_utils::newcsstr("Cancel"),
                il2cpp_utils::MakeDelegate<System::Action_1<int>*>(classof(System::Action_1<int>*), (std::function<void(int)>)[self, success](int btnId) {
                    switch (btnId)
                    {
                    default:
                    case 0: // Continue
                        MultiplayerModeSelectionFlowCoordinator_HandleJoinQuickPlayViewControllerDidFinish(self, success);
                        return;
                    case 1: // Cancel
                        //self->DismissViewController(self->dyn__simpleDialogPromptViewController(), HMUI::ViewController::AnimationDirection::Vertical, nullptr, false);
                        self->ReplaceTopViewController(self->dyn__joinQuickPlayViewController(), nullptr, HMUI::ViewController::AnimationType::In, HMUI::ViewController::AnimationDirection::Vertical);
                        return;
                    }
                    }
                )
            );
            self->ReplaceTopViewController(self->dyn__simpleDialogPromptViewController(), nullptr, HMUI::ViewController::AnimationType::In, HMUI::ViewController::AnimationDirection::Vertical);
        } else MultiplayerModeSelectionFlowCoordinator_HandleJoinQuickPlayViewControllerDidFinish(self, success);
    }

    void Hooks::QuickplayHooks() {
        INSTALL_HOOK(getLogger(), QuickPlaySongPacksDropdown_LazyInit);
        INSTALL_HOOK(getLogger(), MultiplayerModeSelectionFlowCoordinator_HandleJoinQuickPlayViewControllerDidFinish);
        INSTALL_HOOK(getLogger(), JoinQuickPlayViewController_setup);
    }
}

