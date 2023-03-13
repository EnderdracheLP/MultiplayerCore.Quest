#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "Hooks/SessionManagerAndExtendedPlayerHooks.hpp"
#include "Networking/MpPacketSerializer.hpp"
#include "UI/DownloadedSongsViewController.hpp"
#include "UI/CenterScreenLoading.hpp"

#include "Utilities.hpp"

#include "GlobalNamespace/MultiplayerLobbyConnectionController.hpp"
#include "GlobalNamespace/ConnectedPlayerManager.hpp"
#include "GlobalNamespace/BloomFilterUtil.hpp"
#include "GlobalNamespace/MultiplayerLevelLoader.hpp"
#include "GlobalNamespace/PreviewDifficultyBeatmap.hpp"

#include "GlobalNamespace/MultiplayerLevelSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/CenterStageScreenController.hpp"
#include "GlobalNamespace/LobbySetupViewController.hpp"
#include "GlobalNamespace/LevelSelectionNavigationController.hpp"
#include "GlobalNamespace/GameServerPlayerTableCell.hpp"
#include "GlobalNamespace/IMenuRpcManager.hpp"

#include "GlobalNamespace/AdditionalContentModel.hpp"
#include "GlobalNamespace/CreateServerFormData.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "custom-types/shared/register.hpp"
#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

//#include "GlobalNamespace/LobbySetupViewController.hpp"
//#include "GlobalNamespace/LobbySetupViewController_CannotStartGameReason.hpp"
using namespace GlobalNamespace;
using namespace System::Threading::Tasks;
using namespace MultiplayerCore;

#ifndef VERSION
#warning No Version set
#define VERSION "0.1.0"
#endif

// Below define will enable DEBUG Hooks
#define DEBUG

ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* myLogger = new Logger(modInfo, LoggerOptions(false, true));
    return *myLogger;
}


namespace MultiplayerCore {
    // Plugin setup stuff
    GlobalNamespace::MultiplayerSessionManager* _multiplayerSessionManager;
    GlobalNamespace::LobbyPlayersDataModel* lobbyPlayersDataModel;
    Networking::MpPacketSerializer* mpPacketSerializer;
    GlobalNamespace::LobbyGameStateController* lobbyGameStateController;
    LobbySetupViewController* lobbySetupView;

    std::string moddedState = "modded";

    ConstString customLevelSongPackMaskStr("custom_levelpack_CustomLevels");

    StringW getCustomLevelSongPackMaskStr() {
        return customLevelSongPackMaskStr;
    }

    StringW getModdedStateStr() {
        static ConstString moddedStateStr("modded");
        return moddedStateStr;
    }

    StringW getMEStateStr() {
        static ConstString stateStr("ME_Installed");
        return stateStr;
    }

    StringW getNEStateStr() {
        static ConstString stateStr("NE_Installed");
        return stateStr;
    }

    StringW getChromaStateStr() {
        static ConstString stateStr("Chroma_Installed");
        return stateStr;
    }

    bool AllPlayersModded() {
        for (int i = 0; i < _multiplayerSessionManager->connectedPlayers->get_Count(); i++) {
            if (!_multiplayerSessionManager->connectedPlayers->get_Item(i)->HasState(getModdedStateStr())) return false;
        }
        return true;
    }
}

// LobbyPlayersDataModel Activate
MAKE_HOOK_MATCH(LobbyPlayersActivate, &LobbyPlayersDataModel::Activate, void, LobbyPlayersDataModel* self) {
    getLogger().debug("LobbyPlayersActivate Start");
    lobbyPlayersDataModel = self;
    LobbyPlayersActivate(lobbyPlayersDataModel);
    getLogger().debug("LobbyPlayersActivate Done");
}

MAKE_HOOK_MATCH(LobbySetupViewController_DidActivate, &LobbySetupViewController::DidActivate, void, LobbySetupViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    lobbySetupView = self;
    LobbySetupViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    if (getConfig().config["autoDelete"].GetBool() && !DownloadedSongIds.empty()) {
        using namespace RuntimeSongLoader::API;
        std::string hash = DownloadedSongIds.back();
        getLogger().debug("AutoDelete Song with Hash '%s'", hash.c_str());
        std::optional<CustomPreviewBeatmapLevel*> levelOpt = GetLevelByHash(hash);
        if (levelOpt.has_value()) {
            std::string songPath = to_utf8(csstrtostr(levelOpt.value()->get_customLevelPath()));
            getLogger().info("Deleting Song: %s", songPath.c_str());
            DeleteSong(songPath, [&] {
                RefreshSongs(false);
                DownloadedSongIds.pop_back();
                });
            if (lobbyGameStateController) lobbyGameStateController->menuRpcManager->SetIsEntitledToLevel(levelOpt.value()->get_levelID(), EntitlementsStatus::NotDownloaded);
            else getLogger().error("LobbyGameStateController is null!");
        }
    }
}

MAKE_HOOK_MATCH(MultiplayerLobbyConnectionController_CreateParty, &MultiplayerLobbyConnectionController::CreateParty, void, MultiplayerLobbyConnectionController* self, CreateServerFormData data) {
    data.songPacks = SongPackMask::get_all() | SongPackMask(BloomFilterUtil::ToBloomFilter<BitMask128>(getCustomLevelSongPackMaskStr(), 2, 13));
    MultiplayerLobbyConnectionController_CreateParty(self, data);
}

// Show the custom levels tab in multiplayer
// void Setup(::GlobalNamespace::SongPackMask songPackMask, ::GlobalNamespace::BeatmapDifficultyMask allowedBeatmapDifficultyMask, 
// ::ArrayW<::GlobalNamespace::BeatmapCharacteristicSO*> notAllowedCharacteristics, bool hidePacksIfOneOrNone, bool hidePracticeButton, 
// ::StringW actionButtonText, ::GlobalNamespace::IBeatmapLevelPack* levelPackToBeSelectedAfterPresent, 
// ::GlobalNamespace::SelectLevelCategoryViewController::LevelCategory startLevelCategory, bool onlyNotOwned, 
// ::GlobalNamespace::IPreviewBeatmapLevel* beatmapLevelToBeSelectedAfterPresent, bool enableCustomLevels);
MAKE_HOOK_MATCH(LevelSelectionNavigationController_Setup, &LevelSelectionNavigationController::Setup, void, LevelSelectionNavigationController* self,
    SongPackMask songPackMask, BeatmapDifficultyMask allowedBeatmapDifficultyMask, ::ArrayW<BeatmapCharacteristicSO*> notAllowedCharacteristics, 
    bool hidePacksIfOneOrNone, bool hidePracticeButton, ::StringW actionButtonText, IBeatmapLevelPack* levelPackToBeSelectedAfterPresent, 
    SelectLevelCategoryViewController::LevelCategory startLevelCategory, IPreviewBeatmapLevel* beatmapLevelToBeSelectedAfterPresent, 
    bool enableCustomLevels) {
    getLogger().info("LevelSelectionNavigationController_Setup setting custom songs . . .");
    LevelSelectionNavigationController_Setup(self, songPackMask, allowedBeatmapDifficultyMask, notAllowedCharacteristics, hidePacksIfOneOrNone, hidePracticeButton,
        actionButtonText, levelPackToBeSelectedAfterPresent, startLevelCategory, beatmapLevelToBeSelectedAfterPresent, songPackMask.Contains(getCustomLevelSongPackMaskStr()));
}

static bool isMissingLevel = false;

// This hook makes sure to grey-out the play button so that players can't start a level that someone doesn't have.
// This prevents crashes.
MAKE_HOOK_MATCH(LobbySetupViewController_SetPlayersMissingLevelText , &LobbySetupViewController::SetPlayersMissingLevelText, void, LobbySetupViewController* self, StringW playersMissingLevelText) {
    getLogger().info("LobbySetupViewController_SetPlayersMissingLevelText");
    if (!missingLevelText.empty()) {
        getLogger().info("Disabling start game as entitlements missing level text exists . . .");
        isMissingLevel = true;
        playersMissingLevelText = StringW(missingLevelText.c_str());
        self->SetStartGameEnabled(CannotStartGameReason::DoNotOwnSong);
        LobbySetupViewController_SetPlayersMissingLevelText(self, playersMissingLevelText);
        return;
    }
    else if (playersMissingLevelText) {
        getLogger().info("Disabling start game as missing level text exists . . .");
        isMissingLevel = true;
        self->SetStartGameEnabled(CannotStartGameReason::DoNotOwnSong);
    }
    else {
        getLogger().info("Enabling start game as missing level text does not exist . . .");
        isMissingLevel = false;
        self->SetStartGameEnabled(CannotStartGameReason::None);
    }


    LobbySetupViewController_SetPlayersMissingLevelText(self, playersMissingLevelText);
}

#include "Utils/EnumUtils.hpp"

// Prevent the button becoming shown when we're force disabling it, as pressing it would crash
MAKE_HOOK_MATCH(LobbySetupViewController_SetStartGameEnabled, &LobbySetupViewController::SetStartGameEnabled, void, LobbySetupViewController* self, CannotStartGameReason cannotStartGameReason) {
    getLogger().info("LobbySetupViewController_SetStartGameEnabled. Reason: %s", EnumUtils::GetEnumName(cannotStartGameReason).c_str());
    if (isMissingLevel && cannotStartGameReason == CannotStartGameReason::None) {
        getLogger().info("Game attempted to enable the play button when the level was missing, stopping it!");
        cannotStartGameReason = CannotStartGameReason::DoNotOwnSong;
    }
    LobbySetupViewController_SetStartGameEnabled(self, cannotStartGameReason);
}

namespace MultiplayerCore {
    bool HasSong(const std::string& levelId) {
        return static_cast<bool>(RuntimeSongLoader::API::GetLevelById(levelId)) || !IsCustomLevel(levelId);
    }
}

std::vector<std::string> DownloadedSongIds;

#include "System/Threading/CancellationTokenSource.hpp"
#include "GlobalNamespace/LevelGameplaySetupData.hpp"

static bool isDownloading = false;

MAKE_HOOK_MATCH_NO_CATCH(MultiplayerLevelLoader_LoadLevel, &MultiplayerLevelLoader::LoadLevel, void, MultiplayerLevelLoader* self, ILevelGameplaySetupData* gameplaySetupData, float initialStartTime) {
    try {
        if (isDownloading) {
            getLogger().info("Already downloading level, skipping...");
            return;
        }
        if (gameplaySetupData && gameplaySetupData->get_beatmapLevel()) {
            getLogger().debug("LoadLevel PreviewDifficultyBeatmap type: %s", il2cpp_utils::ClassStandardName(reinterpret_cast<Il2CppObject*>(gameplaySetupData->get_beatmapLevel())->klass).c_str());
            if (gameplaySetupData->get_beatmapLevel()->get_beatmapLevel()) {
                getLogger().debug("LoadLevel IPreviewBeatmapLevel type: %s", il2cpp_utils::ClassStandardName(reinterpret_cast<Il2CppObject*>(gameplaySetupData->get_beatmapLevel()->get_beatmapLevel())->klass).c_str());
            }
        }
        std::string levelId = static_cast<std::string>(gameplaySetupData->get_beatmapLevel()->get_beatmapLevel()->get_levelID());
        getLogger().info("Loading Level: %s", levelId.c_str());
        MultiplayerCore::UI::CenterScreenLoading* cslInstance = MultiplayerCore::UI::CenterScreenLoading::get_Instance();
        if (IsCustomLevel(levelId)) {
            if (HasSong(levelId)) {
                getLogger().debug("MultiplayerLevelLoader_LoadLevel, HasSong, calling original");
                if (lobbyGameStateController) lobbyGameStateController->menuRpcManager->SetIsEntitledToLevel(levelId.c_str(), EntitlementsStatus::Ok);
                else getLogger().error("LobbyGameStateController is null!");
                MultiplayerLevelLoader_LoadLevel(self, gameplaySetupData, initialStartTime);
                reinterpret_cast<LevelGameplaySetupData*>(gameplaySetupData)->beatmapLevel->set_beatmapLevel(self->beatmapLevelsModel->GetLevelPreviewForLevelId(StringW(levelId.c_str())));
                //self->dyn__getBeatmapLevelResultTask() = self->dyn__beatmapLevelsModel()->GetBeatmapLevelAsync(StringW(levelId.c_str()), self->dyn__getBeatmapCancellationTokenSource()->get_Token());
                return;
            }
            else {
                isDownloading = true;
                std::string hash = Utilities::GetHash(levelId);
                // TODO: Keep the below in mind, and tweak if necessary.
                // Be mindful of your lambdas, once again.
                // This catpures self as a pointer, could be gc'd.
                // gameplaySetupData as a pointer, could be gc'd.
                // cslInstance, as a pointer, as a singleton so presumably not gc'd, soft restart could change that though.
                // everything else okay.
                std::function<void(float)> beatmapDownloadUpdate = [cslInstance](float downloadProgress) {
                    if (cslInstance) {
                        QuestUI::MainThreadScheduler::Schedule( [cslInstance, downloadProgress] {
                            cslInstance->ShowDownloadingProgress(downloadProgress);
                        });
                    }
                };
                // captures itself hahaha, however it does have to be static to do so, which means that it is only safe becuase of the isDownloading check
                static std::function<void(BeatSaver::Beatmap, int, bool)> onBeatmapDownload;
                onBeatmapDownload = [self, gameplaySetupData, initialStartTime, hash, levelId, beatmapDownloadUpdate](BeatSaver::Beatmap beatmap, int tryNum, bool error) {
                    auto beatmapName = beatmap.GetName();
                    if (error) {
                        // try again if fewer than max tries
                        static const int MAX_TRIES = 2;
                        if(tryNum < MAX_TRIES)
                            BeatSaver::API::DownloadBeatmapAsync(beatmap, [beatmap, tryNum](bool error) { onBeatmapDownload(beatmap, tryNum + 1, error); }, beatmapDownloadUpdate);
                        else
                            getLogger().info("Failed downloading map: %s", beatmapName.c_str());
                    }
                    else {
                        getLogger().info("Downloaded map: %s", beatmapName.c_str());
                        DownloadedSongIds.emplace_back(hash);
                        QuestUI::MainThreadScheduler::Schedule( [self, gameplaySetupData, initialStartTime, hash, levelId] {
                            RuntimeSongLoader::API::RefreshSongs(false,
                                [self, gameplaySetupData, initialStartTime, hash, levelId](const std::vector<GlobalNamespace::CustomPreviewBeatmapLevel*>& songs) {
                                    getLogger().debug("Pointer Check before loading level: self='%p', gameplaySetupData='%p'", self, gameplaySetupData);
                                    self->loaderState = MultiplayerLevelLoader::MultiplayerBeatmapLoaderState::NotLoading;
                                    getLogger().debug("Setting beatmapLevel");
                                    for (GlobalNamespace::CustomPreviewBeatmapLevel* song : songs) {
                                        if (static_cast<std::string>(song->get_levelID()) == levelId) {
                                            getLogger().debug("Found song, setting beatmapLevel");
                                            auto* downloadedSongsGSM = MultiplayerCore::UI::DownloadedSongsViewController::get_Instance();
                                            if (!getConfig().config["autoDelete"].GetBool() && downloadedSongsGSM) downloadedSongsGSM->InsertCell(song);
                                            else if (!getConfig().config["autoDelete"].GetBool()) {
                                                getLogger().warning("DownloadedSongsGSM was null, adding to queue");
                                                MultiplayerCore::UI::DownloadedSongsViewController::mapQueue.push_back(hash);
                                            }
                                            reinterpret_cast<LevelGameplaySetupData*>(gameplaySetupData)->beatmapLevel->set_beatmapLevel(reinterpret_cast<GlobalNamespace::IPreviewBeatmapLevel*>(song));
                                            break;
                                        }
                                    }
                                    // reinterpret_cast<LevelGameplaySetupData*>(gameplaySetupData)->beatmapLevel->set_beatmapLevel(self->beatmapLevelsModel->GetLevelPreviewForLevelId(levelId.c_str()));
                                    getLogger().debug("MultiplayerLevelLoader_LoadLevel, Downloaded, calling original");
                                    MultiplayerLevelLoader_LoadLevel(self, gameplaySetupData, initialStartTime);
                                    if (lobbyGameStateController) lobbyGameStateController->menuRpcManager->SetIsEntitledToLevel(levelId.c_str(), EntitlementsStatus::Ok);
                                    else getLogger().error("LobbyGameStateController is null!");
                                    //self->dyn__getBeatmapLevelResultTask() = self->dyn__beatmapLevelsModel()->GetBeatmapLevelAsync(StringW(levelId.c_str()), self->dyn__getBeatmapCancellationTokenSource()->get_Token());
                                    isDownloading = false;
                                    return;
                                }
                            );
                        } );
                    }
                };
                BeatSaver::API::GetBeatmapByHashAsync(hash,
                    [self, gameplaySetupData, initialStartTime, hash, levelId, cslInstance, beatmapDownloadUpdate](std::optional<BeatSaver::Beatmap> beatmapOpt) {
                        if (beatmapOpt.has_value()) {
                            auto beatmap = beatmapOpt.value();
                            auto beatmapName = beatmap.GetName();
                            getLogger().info("Downloading map: %s", beatmap.GetName().c_str());
                            BeatSaver::API::DownloadBeatmapAsync(beatmap, [beatmap](bool error) { onBeatmapDownload(beatmap, 1, error); }, beatmapDownloadUpdate);
                        }
                    }
                );
            }
        }
        else {
            getLogger().debug("MultiplayerLevelLoader_LoadLevel, calling original as this is a base game beatmap");
            MultiplayerLevelLoader_LoadLevel(self, gameplaySetupData, initialStartTime);
        }
    }
    catch (il2cpp_utils::exceptions::StackTraceException const& e) {
        getLogger().error("REPORT TO ENDER: %s", e.what());
        e.log_backtrace();
        isDownloading = false;
    }
    catch (il2cpp_utils::RunMethodException const& e) {
        getLogger().error("REPORT TO ENDER: %s", e.what());
        e.log_backtrace();
        isDownloading = false;
    }
    catch (const std::exception& e) {
        getLogger().error("REPORT TO ENDER: %s", e.what());
        isDownloading = false;
    }
    catch (...) {
        getLogger().warning("REPORT TO ENDER: An Unknown exception was thrown");
        isDownloading = false;
    }
}

#include "modloader/shared/modloader.hpp"

// Checks entitlement and stalls lobby until fullfilled, unless a game is already in progress.
static ConstString in_gameplay("in_gameplay");
MAKE_HOOK_MATCH(MultiplayerLevelLoader_Tick, &MultiplayerLevelLoader::Tick, void, MultiplayerLevelLoader* self) {
    if (self->loaderState == MultiplayerLevelLoader::MultiplayerBeatmapLoaderState::LoadingBeatmap) {
        MultiplayerLevelLoader_Tick(self);
        if (self->loaderState == MultiplayerLevelLoader::MultiplayerBeatmapLoaderState::WaitingForCountdown) {
            if (lobbyGameStateController) lobbyGameStateController->menuRpcManager->SetIsEntitledToLevel(
                self->gameplaySetupData->get_beatmapLevel()->get_beatmapLevel()->get_levelID(), EntitlementsStatus::Ok);
            else getLogger().error("LobbyGameStateController is null!");
            getLogger().debug("Loaded Level %s", static_cast<std::string>(self->gameplaySetupData->get_beatmapLevel()->get_beatmapLevel()->get_levelID()).c_str());
            // var hash = Utilities.HashForLevelID(_gameplaySetupData.beatmapLevel.beatmapLevel.levelID);
            // if (hash != null)
            // {
            getLogger().debug("Checking extraSongData");
            auto extraSongData = MultiplayerCore::Utils::ExtraSongData::FromPreviewBeatmapLevel(self->gameplaySetupData->get_beatmapLevel()->get_beatmapLevel());
            if (extraSongData)
            {
                getLogger().debug("extraSongData found");
                // std::vector<const MultiplayerCore::Utils::ExtraSongData::DifficultyData>* diffPtr = &extraSongData->_difficulties;
                // auto difficulty = Sombrero::Linq::FirstOrDefault(diffPtr, [self](auto x) {
                //     return x->get_difficulty() == self->gameplaySetupData->get_beatmapLevel()->get_beatmapDifficulty() && 
                //         x->beatmapCharacteristicName == self->gameplaySetupData->get_beatmapLevel()->get_beatmapCharacteristic()->get_name();
                // });

                std::optional<MultiplayerCore::Utils::ExtraSongData::DifficultyData> difficulty;
                getLogger().debug("Checking difficulty size '%lu'", extraSongData->_difficulties.size());
                for (auto& diff : extraSongData->_difficulties) {
                    getLogger().debug("Checking difficulty found '%s' need '%s'", MultiplayerCore::EnumUtils::GetEnumName(diff._difficulty).c_str(), MultiplayerCore::EnumUtils::GetEnumName(self->gameplaySetupData->get_beatmapLevel()->get_beatmapDifficulty()).c_str());
                    getLogger().debug("And beatmapCharacteristicName found '%s' need '%s'", diff._beatmapCharacteristicName.c_str(), static_cast<std::string>(self->gameplaySetupData->get_beatmapLevel()->get_beatmapCharacteristic()->get_serializedName()).c_str());
                    if (diff._difficulty == self->gameplaySetupData->get_beatmapLevel()->get_beatmapDifficulty() &&
                        diff._beatmapCharacteristicName == static_cast<std::string>(self->gameplaySetupData->get_beatmapLevel()->get_beatmapCharacteristic()->get_serializedName())) {
                        getLogger().debug("Found match with difficulty '%s' for beatmapCharacteristicName '%s'", MultiplayerCore::EnumUtils::GetEnumName(diff._difficulty).c_str(), diff._beatmapCharacteristicName.c_str());
                        difficulty = std::move(diff);
                        break;
                    }
                }

                // auto difficulty = extraSongData->_difficulties.FirstOrDefault(x => x._difficulty == _gameplaySetupData.beatmapLevel.beatmapDifficulty && x._beatmapCharacteristicName == _gameplaySetupData.beatmapLevel.beatmapCharacteristic.name);
                // std::vector<std::string>* reqPtr = difficulty ? &(difficulty->additionalDifficultyData->_requirements) : nullptr;
                // ArrayW<std::string> reqPtr = il2cpp_utils::vectorToArray(difficulty->additionalDifficultyData->_requirements);
                // if (reqPtr && !Sombrero::Linq::All(reqPtr, [](auto x) {
                //     return Modloader::getMods().contains(x);
                // }))
                // {
                //     if (lobbyGameStateController) lobbyGameStateController->menuRpcManager->SetIsEntitledToLevel(
                //         self->gameplaySetupData->get_beatmapLevel()->get_beatmapLevel()->get_levelID(), EntitlementsStatus::NotOwned);
                //     self->difficultyBeatmap = nullptr;
                // }
                if (difficulty && difficulty->additionalDifficultyData)
                {
                    for (auto& req : difficulty->additionalDifficultyData->_requirements)
                    {
                        
                        // if (!Modloader::getMods().contains(req))
                        // #define IGNORE_MOD_REQUIREMENTS
                        // if (true)
                        if (!RequirementUtils::GetRequirementInstalled(req) && !RequirementUtils::GetIsForcedSuggestion(req))
                        {
                            getLogger().warning("Attempted to Load Level with Requirement '%s' not installed stopping load", req.c_str());
                            if (lobbyGameStateController) lobbyGameStateController->menuRpcManager->SetIsEntitledToLevel(
                                self->gameplaySetupData->get_beatmapLevel()->get_beatmapLevel()->get_levelID(), EntitlementsStatus::NotOwned);
                            else getLogger().error("LobbyGameStateController is null!");
                            self->difficultyBeatmap = nullptr;
                            break;
                        }
                    }
                }
            }
            else getLogger().warning("No extra song data found for level '%s'", static_cast<std::string>(self->gameplaySetupData->get_beatmapLevel()->get_beatmapLevel()->get_levelID()).c_str());
            // }
        }
    }
    else if (self->loaderState == MultiplayerLevelLoader::MultiplayerBeatmapLoaderState::WaitingForCountdown) {
        std::string LevelID = static_cast<std::string>(self->gameplaySetupData->get_beatmapLevel()->get_beatmapLevel()->get_levelID());
        MultiplayerCore::UI::CenterScreenLoading::playersReady = 0;
        getLogger().debug("Checking all players have the map downloaded/OK entitlement");
        if(HasSong(self->gameplaySetupData->get_beatmapLevel()->get_beatmapLevel()->get_levelID()))
            MultiplayerCore::UI::CenterScreenLoading::playersReady++;
        for (int i = 0; i < _multiplayerSessionManager->connectedPlayers->get_Count(); i++) {
            IConnectedPlayer* p = _multiplayerSessionManager->connectedPlayers->get_Item(i);
            StringW csUserID = p->get_userId();
            std::string UserID =  static_cast<std::string>(csUserID);
            if (entitlementDictionary[UserID][LevelID] == EntitlementsStatus::Ok || p->HasState(in_gameplay)){
                MultiplayerCore::UI::CenterScreenLoading::playersReady++;
            } 
        }
        if (_multiplayerSessionManager->get_syncTime() >= self->startTime && MultiplayerCore::UI::CenterScreenLoading::playersReady == _multiplayerSessionManager->connectedPlayers->get_Count()+1) {
            getLogger().debug("All players have OK entitlement");
            MultiplayerLevelLoader_Tick(self);
        }
    }
    else {
        MultiplayerLevelLoader_Tick(self);
    }
}

MAKE_HOOK_MATCH(LobbyGameStateController_Activate, &LobbyGameStateController::Activate, void, LobbyGameStateController* self) {
    getLogger().debug("LobbyGameStateController_Activate Start");
    lobbyGameStateController = self;
    LobbyGameStateController_Activate(self);
    getLogger().debug("LobbyGameStateController_Activate Done");
}

// TODO: This allows for overwriting entitlement as it doesn't check entitlement for other players selection
MAKE_HOOK_MATCH(GameServerPlayerTableCell_SetData, &GameServerPlayerTableCell::SetData, void, GameServerPlayerTableCell* self, IConnectedPlayer* connectedPlayer, ILobbyPlayerData* playerData, bool hasKickPermissions, bool allowSelection, System::Threading::Tasks::Task_1<AdditionalContentModel::EntitlementStatus>* getLevelEntitlementTask) {
    getLogger().debug("GameServerPlayerTableCell_SetData Set Entitlement Owned");
    getLevelEntitlementTask = Task_1<AdditionalContentModel::EntitlementStatus>::New_ctor(AdditionalContentModel::EntitlementStatus::Owned);
    GameServerPlayerTableCell_SetData(self, connectedPlayer, playerData, hasKickPermissions, allowSelection, getLevelEntitlementTask);
}

MAKE_HOOK_MATCH(CenterStageScreenController_Setup, &CenterStageScreenController::Setup, void, CenterStageScreenController* self, bool showModifiers) {
    CenterStageScreenController_Setup(self, showModifiers);
    if (!self->get_gameObject()->GetComponent<MultiplayerCore::UI::CenterScreenLoading*>())
        self->get_gameObject()->AddComponent<MultiplayerCore::UI::CenterScreenLoading*>();
}

// MAKE_HOOK_MATCH(GameServerLobbyFlowCoordinator_DidActivate, &GameServerLobbyFlowCoordinator::DidActivate, void, GameServerLobbyFlowCoordinator* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
//     GameServerLobbyFlowCoordinator_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
//     if (firstActivation) {
//         getLogger().debug("GameServerLobbyFlowCoordinator_DidActivate firstActivation");

//     }
// }

// MAKE_HOOK_MATCH(MultiplayerModeSelectionFlowCoordinator_BackButtonWasPressed, &MultiplayerModeSelectionFlowCoordinator::BackButtonWasPressed, void, MultiplayerModeSelectionFlowCoordinator* self, HMUI::ViewController* topViewController) {
//     MultiplayerModeSelectionFlowCoordinator_BackButtonWasPressed(self, topViewController);
//     if (topViewController == )
// }

bool HooksInit = false;

HMUI::ViewController* downloadedSongsVC;

// #include "HMUI/ViewController_AnimationType.hpp"

// MAKE_HOOK_MATCH(GameServerLobbyFlowCoordinator_TopViewControllerWillChange, &GameServerLobbyFlowCoordinator::TopViewControllerWillChange, void, GameServerLobbyFlowCoordinator* self, HMUI::ViewController* oldViewController, HMUI::ViewController* newViewController, HMUI::ViewController::AnimationType animationType) {
//     GameServerLobbyFlowCoordinator_TopViewControllerWillChange(self, oldViewController, newViewController, animationType);
//     if (newViewController == downloadedSongsVC) {
//         self->ShowBackButton(true);
//     }
// }

#include "GlobalNamespace/GameServerLobbyFlowCoordinator.hpp"
#include "HMUI/ViewController_AnimationType.hpp"

// MAKE_HOOK_MATCH(GameServerLobbyFlowCoordinator_DidActivate, &GameServerLobbyFlowCoordinator::DidActivate, void, GameServerLobbyFlowCoordinator* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
//     GameServerLobbyFlowCoordinator_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
//     if (addedToHierarchy) {
//         getLogger().debug("GameServerLobbyFlowCoordinator_DidActivate addedToHierarchy");
//         // self->SetBottomScreenViewController(downloadedSongsVC, HMUI::ViewController::AnimationType::In);
//         self->ProvideInitialViewControllers(self->lobbySetupViewController, nullptr, nullptr, downloadedSongsVC, nullptr);
//         getLogger().debug("GameServerLobbyFlowCoordinator_DidActivate addedToHierarchy downloadedSongsVC %p", downloadedSongsVC);
//     }
// }

MAKE_HOOK_MATCH(GameServerLobbyFlowCoordinator_ShowSideViewControllers, &GameServerLobbyFlowCoordinator::ShowSideViewControllers, void, GameServerLobbyFlowCoordinator* self, bool showSideViewControllers, HMUI::ViewController::AnimationType animationType) {
    if (showSideViewControllers) {
        if (!downloadedSongsVC) {
            getLogger().debug("MultiplayerModeSelectionFlowCoordinator_DidActivate Creating DownloadedSongsViewController");
            downloadedSongsVC = QuestUI::BeatSaberUI::CreateViewController<MultiplayerCore::UI::DownloadedSongsViewController*>();
            UnityEngine::GameObject::DontDestroyOnLoad(downloadedSongsVC->get_gameObject());
        }
        getLogger().debug("GameServerLobbyFlowCoordinator_ShowSideViewControllers show");
        self->SetBottomScreenViewController(downloadedSongsVC, animationType);
        getLogger().debug("GameServerLobbyFlowCoordinator_ShowSideViewControllers show downloadedSongsVC %p", downloadedSongsVC);
    } else {
        getLogger().debug("GameServerLobbyFlowCoordinator_ShowSideViewControllers hide");
        self->SetBottomScreenViewController(nullptr, animationType);
        getLogger().debug("GameServerLobbyFlowCoordinator_ShowSideViewControllers hide downloadedSongsVC %p", downloadedSongsVC);
    }
    GameServerLobbyFlowCoordinator_ShowSideViewControllers(self, showSideViewControllers, animationType);
}

#pragma region Debug Logging Transition
#include "GlobalNamespace/MenuTransitionsHelper.hpp"
MAKE_HOOK_FIND_VERBOSE(MenuTransitionsHelper_StartMultiplayerLevel, il2cpp_utils::FindMethodUnsafe("", "MenuTransitionsHelper", "StartMultiplayerLevel", 15), void, GlobalNamespace::MenuTransitionsHelper* self, 
::StringW gameMode, ::GlobalNamespace::IPreviewBeatmapLevel* previewBeatmapLevel, ::GlobalNamespace::BeatmapDifficulty beatmapDifficulty, ::GlobalNamespace::BeatmapCharacteristicSO* beatmapCharacteristic, ::GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, ::GlobalNamespace::ColorScheme* overrideColorScheme, ::GlobalNamespace::GameplayModifiers* gameplayModifiers, ::GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, ::GlobalNamespace::PracticeSettings* practiceSettings, ::StringW backButtonText, bool useTestNoteCutSoundEffects, 
::System::Action* beforeSceneSwitchCallback, ::System::Action_1<::Zenject::DiContainer*>* afterSceneSwitchCallback, ::System::Action_2<::GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO*, ::GlobalNamespace::MultiplayerResultsData*>* levelFinishedCallback, ::System::Action_1<::GlobalNamespace::DisconnectedReason>* didDisconnectCallback, const MethodInfo* info) {
    getLogger().info("StartMultiplayerLevel start");
    getLogger().debug("Check pointers previewBeatmapLevel: %p, beatmapCharacteristic: %p, difficultyBeatmap: %p, overrideColorScheme: %p, gameplayModifiers: %p, playerSpecificSettings: %p, practiceSettings: %p, beforeSceneSwitchCallback: %p, levelFinishedCallback: %p, didDisconnectCallback: %p", 
        previewBeatmapLevel, beatmapCharacteristic, difficultyBeatmap, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, beforeSceneSwitchCallback, levelFinishedCallback, didDisconnectCallback);
    MenuTransitionsHelper_StartMultiplayerLevel(self, gameMode, previewBeatmapLevel, beatmapDifficulty, beatmapCharacteristic, difficultyBeatmap, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects, beforeSceneSwitchCallback, afterSceneSwitchCallback, levelFinishedCallback, didDisconnectCallback, info);
    getLogger().info("StartMultiplayerLevel end");
}

#include "GlobalNamespace/GameScenesManager.hpp"
#include "Zenject/ZenjectSceneLoader.hpp"
MAKE_HOOK_MATCH(GameScenesManager_PushScenes, &GlobalNamespace::GameScenesManager::PushScenes, void, GlobalNamespace::GameScenesManager* self, 
::GlobalNamespace::ScenesTransitionSetupDataSO* scenesTransitionSetupData, float minDuration, ::System::Action* afterMinDurationCallback, ::System::Action_1<::Zenject::DiContainer*>* finishCallback) {
    getLogger().info("PushScenes start");
    getLogger().debug("Check pointers scenesTransitionSetupData: %p, afterMinDurationCallback: %p, finishCallback: %p", scenesTransitionSetupData, afterMinDurationCallback, finishCallback);
    GameScenesManager_PushScenes(self, scenesTransitionSetupData, minDuration, afterMinDurationCallback, finishCallback);
    getLogger().info("PushScenes end");
}
#pragma endregion

#include "GlobalNamespace/MultiplayerModeSelectionFlowCoordinator.hpp"

MAKE_HOOK_MATCH(MultiplayerModeSelectionFlowCoordinator_DidActivate, &MultiplayerModeSelectionFlowCoordinator::DidActivate, void, MultiplayerModeSelectionFlowCoordinator* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    MultiplayerModeSelectionFlowCoordinator_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
    if (firstActivation && !HooksInit) {
        getLogger().debug("MultiplayerModeSelectionFlowCoordinator_DidActivate Registering Custom Types");
        custom_types::Register::AutoRegister();
        getLogger().debug("MultiplayerModeSelectionFlowCoordinator_DidActivate Installing Late Hooks");
        Hooks::InstallLateHooks();
        INSTALL_HOOK(getLogger(), LobbyPlayersActivate);

        INSTALL_HOOK_ORIG(getLogger(), MultiplayerLevelLoader_LoadLevel);
        INSTALL_HOOK_ORIG(getLogger(), MultiplayerLevelLoader_Tick);

        INSTALL_HOOK(getLogger(), LobbyGameStateController_Activate);
        INSTALL_HOOK(getLogger(), LobbySetupViewController_SetPlayersMissingLevelText);
        INSTALL_HOOK(getLogger(), LobbySetupViewController_SetStartGameEnabled);
        INSTALL_HOOK(getLogger(), LobbySetupViewController_DidActivate);

        INSTALL_HOOK(getLogger(), MultiplayerLobbyConnectionController_CreateParty);

        INSTALL_HOOK(getLogger(), LevelSelectionNavigationController_Setup);
        INSTALL_HOOK(getLogger(), CenterStageScreenController_Setup);

        INSTALL_HOOK(getLogger(), GameServerPlayerTableCell_SetData);

        INSTALL_HOOK(getLogger(), GameServerLobbyFlowCoordinator_ShowSideViewControllers);

#pragma region Debug Logging Transition
        INSTALL_HOOK(getLogger(), MenuTransitionsHelper_StartMultiplayerLevel);
        INSTALL_HOOK(getLogger(), GameScenesManager_PushScenes);
#pragma endregion

        // INSTALL_HOOK(getLogger(), GameServerLobbyFlowCoordinator_DidActivate);

        // INSTALL_HOOK(getLogger(), MultiplayerModeSelectionFlowCoordinator_BackButtonWasPressed);
        HooksInit = true;
        getLogger().debug("MultiplayerModeSelectionFlowCoordinator_DidActivate All Late Hooks Installed");
    }
}


void saveDefaultConfig() {
    getLogger().info("Creating config file...");
    ConfigDocument& config = getConfig().config;

    if (config.HasMember("autoDelete") && config["autoDelete"].IsBool() &&
        config.HasMember("MaxPlayers") && config["MaxPlayers"].IsInt() &&
        config.HasMember("CustomsWarning") && config["CustomsWarning"].IsBool() &&
        config.HasMember("LastWarningVersion") && config["LastWarningVersion"].IsString() &&
        config.HasMember("UserIdPrefix") && config["UserIdPrefix"].IsString()) {
        if (config["MaxPlayers"].GetInt() > 250 || config["MaxPlayers"].GetInt() < 2) {
            getLogger().warning("MaxPlayers is set outside of range");
            config["MaxPlayers"].SetInt(std::clamp(config["MaxPlayers"].GetInt(), 2, 250));
        }
        getLogger().info("Config file already exists.");
        return;
    }  

    //config.RemoveAllMembers();
    if (!config.IsObject())
        config.SetObject();
    auto& allocator = config.GetAllocator();

    if (!(config.HasMember("MaxPlayers") && config["MaxPlayers"].IsInt()))
        config.AddMember("MaxPlayers", 10, allocator);
    else {
        if (config["MaxPlayers"].GetInt() > 250 || config["MaxPlayers"].GetInt() < 2) {
            getLogger().warning("MaxPlayers is set outside of range");
            config["MaxPlayers"].SetInt(std::clamp(config["MaxPlayers"].GetInt(), 2, 250));
        }
    }
    if (!(config.HasMember("autoDelete") && config["autoDelete"].IsBool()))
        config.AddMember("autoDelete", false, allocator);
    if (!(config.HasMember("CustomsWarning") && config["CustomsWarning"].IsBool()))
        config.AddMember("CustomsWarning", true, allocator);
    if (!(config.HasMember("LastWarningVersion") && config["LastWarningVersion"].IsString()))
        config.AddMember("LastWarningVersion", "", allocator);
    if (!(config.HasMember("UserIdPrefix") && config["UserIdPrefix"].IsString()))
        config.AddMember("UserIdPrefix", "", allocator);
        
    getConfig().Write();
    getLogger().info("Config file created.");
}

Logger& MultiplayerCore::EnumUtils::getMpCoreLogger() {
    return getLogger();
}

// MAKE_HOOK_NO_CATCH(engrave_tombstone, 0x0, void, int* tombstone_fd, void* param_2, long param_3, int param_4, void* param_5, void* param_6, void* param_7) {
//     engrave_tombstone(tombstone_fd, param_2, param_3, param_4, param_5, param_6, param_7);
//     getLogger().info("engrave_tombstone called, the game crashed");
// }

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;

    // uintptr_t libunity = baseAddr("libunity.so");
    // uintptr_t engrave_tombstoneAddr = findPattern(libunity, "ff 43 04 d1 fc 63 0d a9 f7 5b 0e a9 f5 53 0f a9 f3 7b 10 a9 57 d0 3b d5 e8 16 40 f9 f4 03 02 aa");
    // INSTALL_HOOK_DIRECT(getLogger(), engrave_tombstone, reinterpret_cast<void*>(engrave_tombstoneAddr));

    //getConfig().Load();
    saveDefaultConfig();

    getLogger().info("Completed setup!");
}

#ifdef DEBUG
// For Hooking Debug Loggers
#include "BGNet/Logging/Debug.hpp"
#include "UnityEngine/Debug.hpp"

MAKE_HOOK_MATCH(BGNetDebug_Log, &BGNet::Logging::Debug::Log, void, StringW message) {
    message ? getLogger().WithContext("BGNetDebug::Log").debug("%s", to_utf8(csstrtostr(message)).c_str()) : getLogger().WithContext("BGNetDebug::Log").error("BGNetDebug::Log called with null message");
    BGNetDebug_Log(message);
}

MAKE_HOOK_MATCH(BGNetDebug_LogError, &BGNet::Logging::Debug::LogError, void, StringW message) {
    message ? getLogger().WithContext("BGNetDebug::LogError").error("%s", to_utf8(csstrtostr(message)).c_str()) : getLogger().WithContext("BGNetDebug::LogError").error("BGNetDebug::LogError called with null message");
    BGNetDebug_LogError(message);
}

MAKE_HOOK_MATCH(BGNetDebug_LogException, &BGNet::Logging::Debug::LogException, void, ::System::Exception* exception, StringW message) {
    message ? getLogger().WithContext("BGNetDebug::LogWarning").critical("%s", to_utf8(csstrtostr(message)).c_str()) : getLogger().WithContext("BGNetDebug::LogException").error("BGNetDebug::LogException called with null message");
    BGNetDebug_LogException(exception, message);
}

MAKE_HOOK_MATCH(BGNetDebug_LogWarning, &BGNet::Logging::Debug::LogWarning, void, StringW message) {
    message ? getLogger().WithContext("BGNetDebug::LogWarning").warning("%s", to_utf8(csstrtostr(message)).c_str()) : getLogger().WithContext("BGNetDebug::LogWarning").error("BGNetDebug::LogWarning called with null message");
    BGNetDebug_LogWarning(message);
}

// MAKE_HOOK_FIND_VERBOSE(Debug_Log, il2cpp_utils::FindMethodUnsafe("UnityEngine", "Debug", "Log", 1), void, Il2CppObject* message, const MethodInfo* info) {
// // MAKE_HOOK_MATCH(Debug_Log, &UnityEngine::Debug::Log, void, Il2CppObject* message) {
//     std::optional<Il2CppString*> messageOpt = il2cpp_utils::try_cast<Il2CppString>(message);
//     messageOpt.has_value() ? getLogger().WithContext("UnityEngine::Debug::Log").warning("%s", 
//     to_utf8(csstrtostr(messageOpt.value())).c_str()) : 
//     getLogger().WithContext("UnityEngine::Debug::Log")
//     .error("UnityEngine::Debug::Log can't read message, is message valid? '%s', type of class '%s'", 
//     (bool)messageOpt ? "true" : "false", il2cpp_utils::ClassStandardName(message->klass).c_str());
//     Debug_Log(message, info);
// }

// MAKE_HOOK_FIND_VERBOSE(Debug_LogError, il2cpp_utils::FindMethodUnsafe("UnityEngine", "Debug", "LogError", 1), void, Il2CppObject* message, const MethodInfo* info) {
//     std::optional<Il2CppString*> messageOpt = il2cpp_utils::try_cast<Il2CppString>(message);
//     messageOpt.has_value() ? getLogger().WithContext("UnityEngine::Debug::LogError").warning("%s", 
//     to_utf8(csstrtostr(messageOpt.value())).c_str()) : 
//     getLogger().WithContext("UnityEngine::Debug::LogError")
//     .error("UnityEngine::Debug::LogError can't read message, is message valid? '%s', type of class '%s'", 
//     (bool)messageOpt ? "true" : "false", il2cpp_utils::ClassStandardName(message->klass).c_str());
//     Debug_LogError(message, info);
// }

// MAKE_HOOK_FIND_VERBOSE(Debug_LogWarning, il2cpp_utils::FindMethodUnsafe("UnityEngine", "Debug", "LogWarning", 1), void, Il2CppObject* message, const MethodInfo* info) {
//     std::optional<Il2CppString*> messageOpt = il2cpp_utils::try_cast<Il2CppString>(message);
//     messageOpt.has_value() ? getLogger().WithContext("UnityEngine::Debug::LogError").warning("%s", 
//     to_utf8(csstrtostr(messageOpt.value())).c_str()) : 
//     getLogger().WithContext("UnityEngine::Debug::LogError")
//     .error("UnityEngine::Debug::LogError can't read message, is message valid? '%s', type of class '%s'", 
//     (bool)messageOpt ? "true" : "false", il2cpp_utils::ClassStandardName(message->klass).c_str());
//     Debug_LogWarning(message, info);
// }
#endif

// #include "GlobalNamespace/MultiplayerGameplayAnimator.hpp"
// #include "GlobalNamespace/SimpleColorSO.hpp"

// // Utilities for working with colors
// ColorSO* getColorSO(UnityEngine::Color color) {
//     GlobalNamespace::SimpleColorSO* colorSO = reinterpret_cast<GlobalNamespace::SimpleColorSO*>(
//         UnityEngine::ScriptableObject::CreateInstance(csTypeOf(GlobalNamespace::SimpleColorSO*))
//     );
//     colorSO->SetColor(color);
//     return colorSO;
// }
// ColorSO* getColorSO(float r, float g, float b, float a) {
//     UnityEngine::Color color = { r, g, b, a };
//     return getColorSO(color);
// }

// MAKE_HOOK_MATCH(MultiplayerGameplayAnimator_HandleStateChanged, &GlobalNamespace::MultiplayerGameplayAnimator::HandleStateChanged, void, MultiplayerGameplayAnimator* self, MultiplayerController::State state) {
//     MultiplayerGameplayAnimator_HandleStateChanged(self, state);
//     self->activeLightsColor = getColorSO(0, 0, 0, 0);
//     self->leadingLightsColor = getColorSO(0, 0, 0, 0);
// }


// const MethodInfo* getMenuTransitionsHelper_StartMultiplayerLevel_MethodInfo() {
//     static const MethodInfo* ___internal__method = THROW_UNLESS((::il2cpp_utils::FindMethod("", "MenuTransitionsHelper", "StartMultiplayerLevel", 
//         std::vector<Il2CppClass*>{classof(MenuTransitionsHelper*)}, 
//         ::std::vector<const Il2CppType*>{
//             ::il2cpp_utils::ExtractIndependentType<StringW>(), 
//             ::il2cpp_utils::ExtractIndependentType<GlobalNamespace::IPreviewBeatmapLevel*>(),
//             ::il2cpp_utils::ExtractIndependentType<GlobalNamespace::BeatmapDifficulty*>(),
//             ::il2cpp_utils::ExtractIndependentType<GlobalNamespace::BeatmapCharacteristicSO*>(),
//             ::il2cpp_utils::ExtractIndependentType<GlobalNamespace::IDifficultyBeatmap*>(),
//             ::il2cpp_utils::ExtractIndependentType<GlobalNamespace::ColorScheme*>(),
//             ::il2cpp_utils::ExtractIndependentType<GlobalNamespace::GameplayModifiers*>(),
//             ::il2cpp_utils::ExtractIndependentType<GlobalNamespace::PlayerSpecificSettings*>(),
//             ::il2cpp_utils::ExtractIndependentType<GlobalNamespace::PracticeSettings*>(),
//             ::il2cpp_utils::ExtractIndependentType<::System::Action*>(),
//             ::il2cpp_utils::ExtractIndependentType<::System::Action_1<::Zenject::DiContainer*>*>(),
//             })));
//     return ___internal__method;
// }


// MAKE_HOOK_FIND_VERBOSE(MenuTransitionsHelper_StartMultiplayerLevel, getMenuTransitionsHelper_StartMultiplayerLevel_MethodInfo(), MenuTransitionsHelper*,  const MethodInfo* info)


// MAKE_HOOK_MATCH(MenuTransitionsHelper_StartMultiplayerLevel, &GlobalNamespace::MenuTransitionsHelper::StartMultiplayerLevel, void, GlobalNamespace::MenuTransitionsHelper* self, 
// ::StringW gameMode, ::GlobalNamespace::IPreviewBeatmapLevel* previewBeatmapLevel, ::GlobalNamespace::BeatmapDifficulty beatmapDifficulty, ::GlobalNamespace::BeatmapCharacteristicSO* beatmapCharacteristic, ::GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, ::GlobalNamespace::ColorScheme* overrideColorScheme, ::GlobalNamespace::GameplayModifiers* gameplayModifiers, ::GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, ::GlobalNamespace::PracticeSettings* practiceSettings, ::StringW backButtonText, bool useTestNoteCutSoundEffects, 
// ::System::Action* beforeSceneSwitchCallback, ::System::Action_1<::Zenject::DiContainer*>* afterSceneSwitchCallback, ::System::Action_2<::GlobalNamespace::MultiplayerLevelScenesTransitionSetupDataSO*, ::GlobalNamespace::MultiplayerResultsData*>* levelFinishedCallback, ::System::Action_1<::GlobalNamespace::DisconnectedReason>* didDisconnectCallback)
// {
//     getLogger().info("StartMultiplayerLevel start");
//     getLogger().debug("Check pointers previewBeatmapLevel: %p, beatmapCharacteristic: %p, difficultyBeatmap: %p, overrideColorScheme: %p, gameplayModifiers: %p, playerSpecificSettings: %p, practiceSettings: %p, beforeSceneSwitchCallback: %p, levelFinishedCallback: %p, didDisconnectCallback: %p", previewBeatmapLevel, beatmapCharacteristic, difficultyBeatmap, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, beforeSceneSwitchCallback, levelFinishedCallback, didDisconnectCallback);
//     MenuTransitionsHelper_StartMultiplayerLevel(self, gameMode, previewBeatmapLevel, beatmapDifficulty, beatmapCharacteristic, difficultyBeatmap, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects, beforeSceneSwitchCallback, afterSceneSwitchCallback, levelFinishedCallback, didDisconnectCallback);
//     getLogger().info("StartMultiplayerLevel end");
// }

// Called later on in the game loading - a good time to install function hooks

#if defined(IGNORE_MOD_REQUIREMENTS)
#include "pinkcore/shared/RequirementAPI.hpp"
#endif

#include "System/Enum.hpp"
#include "System/ValueType.hpp"
#include "questui/shared/CustomTypes/Components/List/CustomListTableData.hpp"
#include "UnityEngine/Application.hpp"


extern "C" void load() {
    il2cpp_functions::Init();
  

    // custom_types::Register::ExplicitRegister({QuestUI::CustomListTableData::___TypeRegistration::get(), MultiplayerCore::UI::DownloadedSongsGSM::___TypeRegistration::get()});

    // QuestUI::Register::RegisterGameplaySetupMenu<MultiplayerCore::UI::DownloadedSongsGSM*>(modInfo, "MP Downloaded", QuestUI::Register::Online);

    #if defined(IGNORE_MOD_REQUIREMENTS)
    #warning "Ignoring mod requirements"
    PinkCore::RequirementAPI::RegisterInstalled("Chroma");
    PinkCore::RequirementAPI::RegisterInstalled("Chroma Lighting Events");
    PinkCore::RequirementAPI::RegisterInstalled("Mapping Extensions");
    PinkCore::RequirementAPI::RegisterInstalled("Noodle Extensions");
    #endif

    getLogger().info("MultiplayerCore load start - %s for GameVersion - %s", VERSION, static_cast<std::string>(UnityEngine::Application::get_version()).c_str());
    getLogger().info("Installing early hooks...");

    Hooks::InstallEarlyHooks();
    // INSTALL_HOOK(getLogger(), MultiplayerGameplayAnimator_HandleStateChanged);
    INSTALL_HOOK(getLogger(), MultiplayerModeSelectionFlowCoordinator_DidActivate);

    if (Modloader::getMods().find("BeatTogether") != Modloader::getMods().end()) {
        getLogger().info("Hello BeatTogether!");
    }
    else getLogger().warning("BeatTogether was not found! Is Multiplayer modded?");

#pragma region Debug Hooks
#ifdef DEBUG
#warning Debug Hooks enabled!!!
    INSTALL_HOOK(getLogger(), BGNetDebug_Log);
    INSTALL_HOOK(getLogger(), BGNetDebug_LogError);
    INSTALL_HOOK(getLogger(), BGNetDebug_LogException);
    INSTALL_HOOK(getLogger(), BGNetDebug_LogWarning);

    // INSTALL_HOOK(getLogger(), Debug_Log);
    // INSTALL_HOOK(getLogger(), Debug_LogError);
    // INSTALL_HOOK(getLogger(), Debug_LogWarning);
#endif
#pragma endregion

    getLogger().info("Installed all early hooks!");
}
