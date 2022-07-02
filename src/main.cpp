#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "Hooks/SessionManagerAndExtendedPlayerHooks.hpp"
#include "Networking/MpPacketSerializer.hpp"
#include "UI/DownloadedSongsGSM.hpp"
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
using namespace MultiQuestensions;
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

    bool AllPlayersHaveNE() {
        for (int i = 0; i < _multiplayerSessionManager->connectedPlayers->get_Count(); i++) {
            if (!_multiplayerSessionManager->connectedPlayers->get_Item(i)->HasState(getNEStateStr())) return false;
        }
        return true;
    }

    bool AllPlayersHaveME() {
        for (int i = 0; i < _multiplayerSessionManager->connectedPlayers->get_Count(); i++) {
            if (!_multiplayerSessionManager->connectedPlayers->get_Item(i)->HasState(getMEStateStr())) return false;
        }
        return true;
    }

    bool AllPlayersHaveChroma() {
        for (int i = 0; i < _multiplayerSessionManager->connectedPlayers->get_Count(); i++) {
            if (!_multiplayerSessionManager->connectedPlayers->get_Item(i)->HasState(getChromaStateStr())) return false;
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
        }
    }
}

MAKE_HOOK_MATCH(MultiplayerLobbyConnectionController_CreateParty, &MultiplayerLobbyConnectionController::CreateParty, void, MultiplayerLobbyConnectionController* self, CreateServerFormData data) {
    data.songPacks = SongPackMask::get_all() | SongPackMask(BloomFilterUtil::ToBloomFilter<BitMask128>(getCustomLevelSongPackMaskStr(), 2, 13));
    MultiplayerLobbyConnectionController_CreateParty(self, data);
}

// Show the custom levels tab in multiplayer
MAKE_HOOK_MATCH(LevelSelectionNavigationController_Setup, &LevelSelectionNavigationController::Setup, void, LevelSelectionNavigationController* self,
    SongPackMask songPackMask, BeatmapDifficultyMask allowedBeatmapDifficultyMask, ::ArrayW<BeatmapCharacteristicSO*> notAllowedCharacteristics, 
    bool hidePacksIfOneOrNone, bool hidePracticeButton, ::StringW actionButtonText, IBeatmapLevelPack* levelPackToBeSelectedAfterPresent, 
    SelectLevelCategoryViewController::LevelCategory startLevelCategory, IPreviewBeatmapLevel* beatmapLevelToBeSelectedAfterPresent, bool enableCustomLevels) {
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

#include "CodegenExtensions/EnumUtils.hpp"

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
                BeatSaver::API::GetBeatmapByHashAsync(hash,
                    [self, gameplaySetupData, initialStartTime, hash, levelId, cslInstance](std::optional<BeatSaver::Beatmap> beatmapOpt) {
                        if (beatmapOpt.has_value()) {
                            auto beatmap = beatmapOpt.value();
                            auto beatmapName = beatmap.GetName();
                            getLogger().info("Downloading map: %s", beatmap.GetName().c_str());
                            BeatSaver::API::DownloadBeatmapAsync(beatmap,
                                [self, gameplaySetupData, initialStartTime, beatmapName, hash, levelId, beatmap, cslInstance](bool error) {
                                    if (error) {
                                        getLogger().info("Failed downloading map retrying: %s", beatmapName.c_str());
                                        BeatSaver::API::DownloadBeatmapAsync(beatmap,
                                            [self, gameplaySetupData, initialStartTime, beatmapName, hash, levelId](bool error) {
                                                if (error) {
                                                    getLogger().info("Failed downloading map: %s", beatmapName.c_str());
                                                }
                                                else {
                                                    getLogger().info("Downloaded map: %s", beatmapName.c_str());
                                                    DownloadedSongIds.emplace_back(hash);
                                                    QuestUI::MainThreadScheduler::Schedule(
                                                        [self, gameplaySetupData, initialStartTime, hash, levelId] {
                                                            RuntimeSongLoader::API::RefreshSongs(false,
                                                                [self, gameplaySetupData, initialStartTime, hash, levelId](const std::vector<GlobalNamespace::CustomPreviewBeatmapLevel*>& songs) {
                                                                    auto* downloadedSongsGSM = MultiQuestensions::UI::DownloadedSongsGSM::get_Instance();
                                                                    if (!getConfig().config["autoDelete"].GetBool() && downloadedSongsGSM) downloadedSongsGSM->InsertCell(hash);
                                                                    else if (!getConfig().config["autoDelete"].GetBool()) {
                                                                        getLogger().warning("DownloadedSongsGSM was null, adding to queue");
                                                                        MultiQuestensions::UI::DownloadedSongsGSM::mapQueue.push_back(hash);
                                                                    }
                                                                    getLogger().debug("Pointer Check before loading level: self='%p', gameplaySetupData='%p'", self, gameplaySetupData);
                                                                    self->loaderState = MultiplayerLevelLoader::MultiplayerBeatmapLoaderState::NotLoading;
                                                                    getLogger().debug("MultiplayerLevelLoader_LoadLevel, Downloaded, calling original");
                                                                    reinterpret_cast<LevelGameplaySetupData*>(gameplaySetupData)->beatmapLevel->set_beatmapLevel(self->beatmapLevelsModel->GetLevelPreviewForLevelId(levelId.c_str()));
                                                                    MultiplayerLevelLoader_LoadLevel(self, gameplaySetupData, initialStartTime);
                                                                    if (lobbyGameStateController) lobbyGameStateController->menuRpcManager->SetIsEntitledToLevel(levelId.c_str(), EntitlementsStatus::Ok);
                                                                    //self->dyn__getBeatmapLevelResultTask() = self->dyn__beatmapLevelsModel()->GetBeatmapLevelAsync(StringW(levelId.c_str()), self->dyn__getBeatmapCancellationTokenSource()->get_Token());
                                                                    isDownloading = false;
                                                                    return;
                                                                }
                                                            );
                                                        }
                                                    );
                                                }
                                            }, [cslInstance](float downloadProgress) {
                                                if (cslInstance) {
                                                    QuestUI::MainThreadScheduler::Schedule(
                                                        [cslInstance, downloadProgress] {
                                                            cslInstance->ShowDownloadingProgress(downloadProgress);
                                                        });
                                                }
                                            }
                                            );
                                    }
                                    else {
                                        getLogger().info("Downloaded map: %s", beatmapName.c_str());
                                        DownloadedSongIds.emplace_back(hash);
                                        QuestUI::MainThreadScheduler::Schedule(
                                            [self, gameplaySetupData, initialStartTime, hash, levelId] {
                                                RuntimeSongLoader::API::RefreshSongs(false,
                                                    [self, gameplaySetupData, initialStartTime, hash, levelId](const std::vector<GlobalNamespace::CustomPreviewBeatmapLevel*>& songs) {
                                                        auto* downloadedSongsGSM = MultiQuestensions::UI::DownloadedSongsGSM::get_Instance();
                                                        if (!getConfig().config["autoDelete"].GetBool() && downloadedSongsGSM) downloadedSongsGSM->InsertCell(hash);
                                                        else if (!getConfig().config["autoDelete"].GetBool()) {
                                                            getLogger().warning("DownloadedSongsGSM was null, adding to queue");
                                                            MultiQuestensions::UI::DownloadedSongsGSM::mapQueue.push_back(hash);
                                                        }
                                                        getLogger().debug("Pointer Check before loading level: self='%p', gameplaySetupData='%p'", self, gameplaySetupData);
                                                        self->loaderState = MultiplayerLevelLoader::MultiplayerBeatmapLoaderState::NotLoading;
                                                        getLogger().debug("MultiplayerLevelLoader_LoadLevel, Downloaded, calling original");
                                                        reinterpret_cast<LevelGameplaySetupData*>(gameplaySetupData)->beatmapLevel->set_beatmapLevel(self->beatmapLevelsModel->GetLevelPreviewForLevelId(levelId.c_str()));
                                                        MultiplayerLevelLoader_LoadLevel(self, gameplaySetupData, initialStartTime);
                                                        if (lobbyGameStateController) lobbyGameStateController->menuRpcManager->SetIsEntitledToLevel(levelId.c_str(), EntitlementsStatus::Ok);
                                                        //self->dyn__getBeatmapLevelResultTask() = self->dyn__beatmapLevelsModel()->GetBeatmapLevelAsync(StringW(levelId.c_str()), self->dyn__getBeatmapCancellationTokenSource()->get_Token());
                                                        isDownloading = false;
                                                        return;
                                                    }
                                                );
                                            }
                                        );
                                    }
                                }, [cslInstance](float downloadProgress) {
                                    if (cslInstance) {
                                        QuestUI::MainThreadScheduler::Schedule(
                                            [cslInstance, downloadProgress] {
                                                cslInstance->ShowDownloadingProgress(downloadProgress);
                                            });
                                    }
                                }
                                );
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
    }
    catch (il2cpp_utils::RunMethodException const& e) {
        getLogger().error("REPORT TO ENDER: %s", e.what());
        e.log_backtrace();
    }
    catch (const std::exception& e) {
        getLogger().error("REPORT TO ENDER: %s", e.what());
    }
    catch (...) {
        getLogger().warning("REPORT TO ENDER: An Unknown exception was thrown");
    }
    isDownloading = false;
}

// Checks entitlement and stalls lobby until fullfilled, unless a game is already in progress.
static ConstString in_gameplay("in_gameplay");
MAKE_HOOK_MATCH(MultiplayerLevelLoader_Tick, &MultiplayerLevelLoader::Tick, void, MultiplayerLevelLoader* self) {
    if (self->loaderState == MultiplayerLevelLoader::MultiplayerBeatmapLoaderState::LoadingBeatmap) {
        MultiplayerLevelLoader_Tick(self);
        if (self->loaderState == MultiplayerLevelLoader::MultiplayerBeatmapLoaderState::WaitingForCountdown) {
            if (lobbyGameStateController) lobbyGameStateController->menuRpcManager->SetIsEntitledToLevel(
                self->gameplaySetupData->get_beatmapLevel()->get_beatmapLevel()->get_levelID(), EntitlementsStatus::Ok);
            getLogger().debug("Loaded Level %s", static_cast<std::string>(self->gameplaySetupData->get_beatmapLevel()->get_beatmapLevel()->get_levelID()).c_str());
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
        if (MultiplayerCore::UI::CenterScreenLoading::playersReady == _multiplayerSessionManager->connectedPlayers->get_Count()+1) {
            getLogger().debug("All players have OK entitilement");
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


void saveDefaultConfig() {
    getLogger().info("Creating config file...");
    ConfigDocument& config = getConfig().config;

    if (config.HasMember("autoDelete") && config["autoDelete"].IsBool() &&
        config.HasMember("MaxPlayers") && config["MaxPlayers"].IsInt() &&
        config.HasMember("CustomsWarning") && config["CustomsWarning"].IsBool() &&
        config.HasMember("LastWarningVersion") && config["LastWarningVersion"].IsString()) {
        if (config["MaxPlayers"].GetInt() > 254 || config["MaxPlayers"].GetInt() < 2) {
            getLogger().warning("MaxPlayers is set outside of range");
            config["MaxPlayers"].SetInt(std::clamp(config["MaxPlayers"].GetInt(), 2, 254));
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
        if (config["MaxPlayers"].GetInt() > 254 || config["MaxPlayers"].GetInt() < 2) {
            getLogger().warning("MaxPlayers is set outside of range");
            config["MaxPlayers"].SetInt(std::clamp(config["MaxPlayers"].GetInt(), 2, 254));
        }
    }
    if (!(config.HasMember("autoDelete") && config["autoDelete"].IsBool()))
        config.AddMember("autoDelete", false, allocator);
    if (!(config.HasMember("CustomsWarning") && config["CustomsWarning"].IsBool()))
        config.AddMember("CustomsWarning", true, allocator);
    if (!(config.HasMember("LastWarningVersion") && config["LastWarningVersion"].IsString()))
        config.AddMember("LastWarningVersion", "", allocator);
        
    getConfig().Write();
    getLogger().info("Config file created.");
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;

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

MAKE_HOOK_FIND_VERBOSE(Debug_Log, il2cpp_utils::FindMethodUnsafe("UnityEngine", "Debug", "Log", 1), void, Il2CppObject* message) {
// MAKE_HOOK_MATCH(Debug_Log, &UnityEngine::Debug::Log, void, Il2CppObject* message) {
    std::optional<Il2CppString*> messageOpt = il2cpp_utils::try_cast<Il2CppString>(message);
    messageOpt.has_value() ? getLogger().WithContext("UnityEngine::Debug::Log").warning("%s", 
    to_utf8(csstrtostr(messageOpt.value())).c_str()) : 
    getLogger().WithContext("UnityEngine::Debug::Log")
    .error("UnityEngine::Debug::Log can't read message, is message valid? '%s', type of class '%s'", 
    (bool)messageOpt ? "true" : "false", il2cpp_utils::ClassStandardName(message->klass).c_str());
    Debug_Log(message);
}

MAKE_HOOK_FIND_VERBOSE(Debug_LogError, il2cpp_utils::FindMethodUnsafe("UnityEngine", "Debug", "LogError", 1), void, Il2CppObject* message) {
    std::optional<Il2CppString*> messageOpt = il2cpp_utils::try_cast<Il2CppString>(message);
    messageOpt.has_value() ? getLogger().WithContext("UnityEngine::Debug::LogError").warning("%s", 
    to_utf8(csstrtostr(messageOpt.value())).c_str()) : 
    getLogger().WithContext("UnityEngine::Debug::LogError")
    .error("UnityEngine::Debug::LogError can't read message, is message valid? '%s', type of class '%s'", 
    (bool)messageOpt ? "true" : "false", il2cpp_utils::ClassStandardName(message->klass).c_str());
    Debug_LogError(message);
}

MAKE_HOOK_FIND_VERBOSE(Debug_LogWarning, il2cpp_utils::FindMethodUnsafe("UnityEngine", "Debug", "LogWarning", 1), void, Il2CppObject* message) {
    std::optional<Il2CppString*> messageOpt = il2cpp_utils::try_cast<Il2CppString>(message);
    messageOpt.has_value() ? getLogger().WithContext("UnityEngine::Debug::LogError").warning("%s", 
    to_utf8(csstrtostr(messageOpt.value())).c_str()) : 
    getLogger().WithContext("UnityEngine::Debug::LogError")
    .error("UnityEngine::Debug::LogError can't read message, is message valid? '%s', type of class '%s'", 
    (bool)messageOpt ? "true" : "false", il2cpp_utils::ClassStandardName(message->klass).c_str());
    Debug_LogWarning(message);
}
#endif

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    custom_types::Register::AutoRegister();

    QuestUI::Register::RegisterGameplaySetupMenu<MultiQuestensions::UI::DownloadedSongsGSM*>(modInfo, "MP Downloaded", QuestUI::Register::Online);

    getLogger().info("Installing hooks...");
    Hooks::Install_Hooks();
    INSTALL_HOOK(getLogger(), LobbyPlayersActivate);

    INSTALL_HOOK_ORIG(getLogger(), MultiplayerLevelLoader_LoadLevel);
    INSTALL_HOOK_ORIG(getLogger(), MultiplayerLevelLoader_Tick);
    if (Modloader::getMods().find("BeatTogether") != Modloader::getMods().end()) {
        getLogger().info("Hello BeatTogether!");
    }
    else getLogger().warning("BeatTogether was not found! Is Multiplayer modded?");

    INSTALL_HOOK(getLogger(), LobbyGameStateController_Activate);
    INSTALL_HOOK(getLogger(), LobbySetupViewController_SetPlayersMissingLevelText);
    INSTALL_HOOK(getLogger(), LobbySetupViewController_SetStartGameEnabled);
    INSTALL_HOOK(getLogger(), LobbySetupViewController_DidActivate);

    INSTALL_HOOK(getLogger(), MultiplayerLobbyConnectionController_CreateParty);

    INSTALL_HOOK(getLogger(), LevelSelectionNavigationController_Setup);
    INSTALL_HOOK(getLogger(), CenterStageScreenController_Setup);

#pragma region Debug Hooks
#ifdef DEBUG
#warning Debug Hooks enabled!!!
    INSTALL_HOOK(getLogger(), BGNetDebug_Log);
    INSTALL_HOOK(getLogger(), BGNetDebug_LogError);
    INSTALL_HOOK(getLogger(), BGNetDebug_LogException);
    INSTALL_HOOK(getLogger(), BGNetDebug_LogWarning);

    INSTALL_HOOK(getLogger(), Debug_Log);
    INSTALL_HOOK(getLogger(), Debug_LogError);
    INSTALL_HOOK(getLogger(), Debug_LogWarning);
#endif
#pragma endregion


    INSTALL_HOOK(getLogger(), GameServerPlayerTableCell_SetData);

    getLogger().info("Installed all hooks!");
}
