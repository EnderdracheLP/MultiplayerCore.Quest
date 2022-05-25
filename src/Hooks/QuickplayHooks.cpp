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
        gotSongPackOverrides = (self->quickPlaySongPacksOverride != nullptr);
        QuickPlaySongPacksDropdown_LazyInit(self);
    }

    //Adds All dificulty to quickplay
    MAKE_HOOK_MATCH(JoinQuickPlayViewController_setup, &JoinQuickPlayViewController::Setup, void, JoinQuickPlayViewController* self, GlobalNamespace::QuickPlaySetupData* quickPlaySetupData, ::GlobalNamespace::MultiplayerModeSettings* multiplayerModeSettings){
        self->beatmapDifficultyDropdown->set_includeAllDifficulties(true);
        
        JoinQuickPlayViewController_setup(self, quickPlaySetupData, multiplayerModeSettings);
        self->beatmapDifficultyDropdown->simpleTextDropdown->SelectCellWithIdx(0);
    }

    //Adds warning screen about custom quickplay
    MAKE_HOOK_MATCH(MultiplayerModeSelectionFlowCoordinator_HandleJoinQuickPlayViewControllerDidFinish, &MultiplayerModeSelectionFlowCoordinator::HandleJoinQuickPlayViewControllerDidFinish, void, MultiplayerModeSelectionFlowCoordinator* self, bool success) {
        Il2CppString* levelPackName = self->joinQuickPlayViewController->multiplayerModeSettings->quickPlaySongPackMaskSerializedName;
        if ((getConfig().config["CustomsWarning"].GetBool() || getConfig().config["LastWarningVersion"].GetString() != modInfo.version) && success && 
            self->songPackMaskModel->ToSongPackMask(
                levelPackName
            ).Contains(
                getCustomLevelSongPackMaskStr())
            ) {
            self->simpleDialogPromptViewController->Init(
                il2cpp_utils::newcsstr("Custom Song Quickplay"),
                il2cpp_utils::newcsstr("<color=#EB4949>This category includes songs of varying difficulty.\nIt may be more enjoyable to play in a private lobby with friends."),
                il2cpp_utils::newcsstr("Continue"),
                il2cpp_utils::newcsstr("Cancel"),
                il2cpp_utils::MakeDelegate<System::Action_1<int>*>(classof(System::Action_1<int>*), (std::function<void(int)>)[self, success](int btnId) {
                    switch (btnId)
                    {
                    default:
                    case 0: // Continue
                        getLogger().debug("Warned once, now removing");
                        getConfig().config["CustomsWarning"].SetBool(false);
                        getConfig().config["LastWarningVersion"].SetString(modInfo.version, getConfig().config.GetAllocator());
                        getConfig().Write();
                        MultiplayerModeSelectionFlowCoordinator_HandleJoinQuickPlayViewControllerDidFinish(self, success);
                        return;
                    case 1: // Cancel
                        //self->DismissViewController(self->dyn__simpleDialogPromptViewController(), HMUI::ViewController::AnimationDirection::Vertical, nullptr, false);
                        self->ReplaceTopViewController(self->joinQuickPlayViewController, nullptr, HMUI::ViewController::AnimationType::In, HMUI::ViewController::AnimationDirection::Vertical);
                        return;
                    }
                    }
                )
            );
            self->ReplaceTopViewController(self->simpleDialogPromptViewController, nullptr, HMUI::ViewController::AnimationType::In, HMUI::ViewController::AnimationDirection::Vertical);
        } else MultiplayerModeSelectionFlowCoordinator_HandleJoinQuickPlayViewControllerDidFinish(self, success);
    }

    void Hooks::QuickplayHooks() {
        INSTALL_HOOK(getLogger(), QuickPlaySongPacksDropdown_LazyInit);
        INSTALL_HOOK(getLogger(), JoinQuickPlayViewController_setup);

        if(getConfig().config["CustomsWarning"].GetBool() || getConfig().config["LastWarningVersion"].GetString() != modInfo.version)
            INSTALL_HOOK(getLogger(), MultiplayerModeSelectionFlowCoordinator_HandleJoinQuickPlayViewControllerDidFinish);
    }
}

