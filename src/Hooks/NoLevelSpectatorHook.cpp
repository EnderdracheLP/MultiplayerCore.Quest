#include "hooking.hpp"
#include "logging.hpp"

#include "Utilities.hpp"
#include "Objects/MpPlayersDataModel.hpp"
#include "Beatmaps/NoInfoBeatmapLevel.hpp"
using namespace MultiplayerCore;

#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "custom-types/shared/delegate.hpp"

#include "GlobalNamespace/LobbyGameStateController.hpp"
#include "GlobalNamespace/ILevelGameplaySetupData.hpp"
#include "GlobalNamespace/IBeatmapLevelData.hpp"
#include "GlobalNamespace/MultiplayerLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/PlayerDataModel.hpp"
#include "GlobalNamespace/PlayerData.hpp"
#include "GlobalNamespace/MenuTransitionsHelper.hpp"
#include "GlobalNamespace/ColorSchemesSettings.hpp"
#include "GlobalNamespace/LevelBar.hpp"
#include "BGLib/Polyglot/Localization.hpp"
#include "System/Action.hpp"
#include "System/Action_2.hpp"
#include "System/Action_1.hpp"

static Beatmaps::Providers::MpBeatmapLevelProvider* mpLevelProvider = nullptr;

MAKE_AUTO_HOOK_ORIG_MATCH(LobbyGameStateController_StartMultiplayerLevel, &::GlobalNamespace::LobbyGameStateController::StartMultiplayerLevel, void, GlobalNamespace::LobbyGameStateController* self, GlobalNamespace::ILevelGameplaySetupData* gameplaySetupData, GlobalNamespace::IBeatmapLevelData* beatmapLevelData, System::Action* beforeSceneSwitchCallback) {
    INFO("LobbyGameStateController_StartMultiplayerLevel");
    auto mpPlayerDataModel = il2cpp_utils::try_cast<Objects::MpPlayersDataModel>(self->_lobbyPlayersDataModel).value_or(nullptr);
    mpLevelProvider = mpPlayerDataModel ? mpPlayerDataModel->_beatmapLevelProvider : mpLevelProvider;
    static GlobalNamespace::BeatmapKey key;
    key = gameplaySetupData ? gameplaySetupData->beatmapKey : GlobalNamespace::BeatmapKey();
    auto hash  = key.IsValid() ? HashFromLevelID(key.levelId) : "";

    DEBUG("Check gameplaySetupData ptr {}, beatmapLevelData ptr {}, mpPlayerDataModel ptr {}, mpLevelProvider ptr {}, hash {}", fmt::ptr(gameplaySetupData), fmt::ptr(beatmapLevelData), fmt::ptr(mpPlayerDataModel), fmt::ptr(mpLevelProvider), hash);

    if (gameplaySetupData && !beatmapLevelData && mpLevelProvider && !hash.empty()) {
        WARNING("No beatmapLevelData, attempting to go into spectator with our level type");
        // Use our custom BeatmapLevelProvider
        // TODO: Maybe use a CT for this but doesn't seem worth the effort for just one hook
        std::shared_future beatmapLevelFut = mpLevelProvider->GetBeatmapAsync(hash);
        self->countdownStarted = false;
        self->StopListeningToGameStart(); // Ensure we stop listening for the start event while we run our start task

        BSML::MainThreadScheduler::AwaitFuture(beatmapLevelFut, [beatmapLevelFut, hash, self, gameplaySetupData, beatmapLevelData, beforeSceneSwitchCallback]() mutable -> void {
            if (self->countdownStarted) return; // Another countdown has started, don't start the level

            GlobalNamespace::BeatmapLevel* beatmapLevel = beatmapLevelFut.get();
            if (!beatmapLevel) {
                DEBUG("No beatmapLevel found, creating a NoInfoBeatmapLevel");
                beatmapLevel = Beatmaps::NoInfoBeatmapLevel::New_ctor(hash);
            }
            // GlobalNamespace::BeatmapLevel* beatmapLevel = Beatmaps::NoInfoBeatmapLevel::New_ctor(hash);
            mpLevelProvider->AddBasicBeatmapDataToLevel(beatmapLevel, key);

            static ConstString gameModeText = "Multiplayer";
            self->_menuTransitionsHelper->StartMultiplayerLevel(gameModeText, byref(key), beatmapLevel, beatmapLevelData, 
                self->_playerDataModel->playerData->colorSchemesSettings->GetOverrideColorScheme(), gameplaySetupData->gameplayModifiers, 
                self->_playerDataModel->playerData->playerSpecificSettings, nullptr, BGLib::Polyglot::Localization::Get("BUTTON_MENU"), 
                false, 
                beforeSceneSwitchCallback,
                custom_types::MakeDelegate<System::Action_2<::UnityW<GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO>, GlobalNamespace::MultiplayerResultsData*>*>(
                    self, 
                    (std::function<void(GlobalNamespace::LobbyGameStateController*, GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO*, GlobalNamespace::MultiplayerResultsData*)>)&GlobalNamespace::LobbyGameStateController::HandleMultiplayerLevelDidFinish
                ),
                custom_types::MakeDelegate<System::Action_1<GlobalNamespace::DisconnectedReason>*>(
                    self, 
                    (std::function<void(GlobalNamespace::LobbyGameStateController*, GlobalNamespace::DisconnectedReason)>)&GlobalNamespace::LobbyGameStateController::HandleMultiplayerLevelDidDisconnect
                )
            );
        });
        return;
    } // Safety check to avoid crashing the game, we leave the lobby then
    else if (!beatmapLevelData && !mpLevelProvider && !hash.empty()) {
        WARNING("No beatmapLevelData and no MpBeatmapLevelProvider, cannot start level, disconnecting...");
        self->HandleMultiplayerLevelDidDisconnect(GlobalNamespace::DisconnectedReason::ClientConnectionClosed);
        return;
    }
    LobbyGameStateController_StartMultiplayerLevel(self, gameplaySetupData, beatmapLevelData, beforeSceneSwitchCallback);
}

MAKE_AUTO_HOOK_ORIG_MATCH(LevelBar_Setup, static_cast<void (::GlobalNamespace::LevelBar::*)(::GlobalNamespace::BeatmapKey)>(&::GlobalNamespace::LevelBar::Setup), void, GlobalNamespace::LevelBar* self, ::GlobalNamespace::BeatmapKey beatmapKey) {
    INFO("LevelBar_Setup");
    auto hash = HashFromLevelID(beatmapKey.levelId);
    if (mpLevelProvider && !hash.empty()) {
        std::shared_future beatmapLevelFut = mpLevelProvider->GetBeatmapAsync(hash);
        BSML::MainThreadScheduler::AwaitFuture(beatmapLevelFut, [beatmapLevelFut, self, hash, beatmapKey]() mutable -> void {
            GlobalNamespace::BeatmapLevel* beatmapLevel = beatmapLevelFut.get();
            if (!beatmapLevel) {
                beatmapLevel = Beatmaps::NoInfoBeatmapLevel::New_ctor(hash);
            }
            self->SetupData(beatmapLevel, beatmapKey.difficulty, beatmapKey.beatmapCharacteristic);
        });
        return;
    }
    LevelBar_Setup(self, beatmapKey);
}