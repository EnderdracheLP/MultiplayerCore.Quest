#include "UI/MpPerPlayerUI.hpp"
#include "Beatmaps/Abstractions/MpBeatmapLevel.hpp"
#include "Utils/EnumUtils.hpp"
#include "Utilities.hpp"

#include "GlobalNamespace/LobbySetupViewController.hpp"
#include "GlobalNamespace/ILobbyGameStateController.hpp"
#include "GlobalNamespace/IMultiplayerSessionManager.hpp"
#include "GlobalNamespace/MultiplayerLobbyState.hpp"
#include "GlobalNamespace/LevelSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/MultiplayerLevelSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/BeatmapKey.hpp"
#include "GlobalNamespace/ServerPlayerListViewController.hpp"
#include "GlobalNamespace/LobbyPlayerPermissionsModel.hpp"
#include "GlobalNamespace/LobbyPlayersDataModel.hpp"
#include "GlobalNamespace/LevelGameplaySetupData.hpp"
#include "GlobalNamespace/UnifiedNetworkPlayerModel.hpp"
#include "GlobalNamespace/BeatmapLevelSelectionMask.hpp"
#include "GlobalNamespace/BeatmapDifficultyMask.hpp"
#include "GlobalNamespace/BeatmapDifficultyMaskExtensions.hpp"
#include "GlobalNamespace/ILobbyPlayersDataModel.hpp"

#include "System/Action.hpp"
#include "System/Collections/Generic/List_1.hpp"
#include "System/Collections/Generic/IEnumerable_1.hpp"
#include "System/Collections/Generic/IEnumerator_1.hpp"

#include "custom-types/shared/delegate.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"

#include "assets.hpp"
#include "logging.hpp"

DEFINE_TYPE(MultiplayerCore::UI, MpPerPlayerUI);

namespace MultiplayerCore::UI {
    void MpPerPlayerUI::ctor(
            GlobalNamespace::GameServerLobbyFlowCoordinator* gameServerLobbyFlowCoordinator,
            GlobalNamespace::BeatmapLevelsModel* beatmapLevelsModel,
            GlobalNamespace::IMultiplayerSessionManager* sessionManager,
            MultiplayerCore::Beatmaps::Providers::MpBeatmapLevelProvider* beatmapLevelProvider,
            MultiplayerCore::Networking::MpPacketSerializer* packetSerializer
        ) {
        // Constructor
        _gameServerLobbyFlowCoordinator = gameServerLobbyFlowCoordinator;
        _lobbyViewController = gameServerLobbyFlowCoordinator->_lobbySetupViewController;
        _gameStateController = gameServerLobbyFlowCoordinator->_lobbyGameStateController;
        _beatmapLevelsModel = beatmapLevelsModel;
        auto sessManOpt = il2cpp_utils::try_cast<GlobalNamespace::MultiplayerSessionManager>(sessionManager);
        if (!sessManOpt.has_value()) {
            CRITICAL("Failed to cast IMultiplayerSessionManager to MultiplayerSessionManager, throwing runtime_error...");
            throw std::runtime_error("Failed to cast IMultiplayerSessionManager to MultiplayerSessionManager");
        }
        _multiplayerSessionManager = sessManOpt.value();
        _beatmapLevelProvider = beatmapLevelProvider;
        _packetSerializer = packetSerializer;
    }

    void MpPerPlayerUI::Initialize() {
        // Initialize
        _allowedDifficulties = ListW<StringW>::New();
        // UI Elements
        // DifficultySelector
        BSML::parse_and_construct(Assets::MpDifficultySelector_bsml, _lobbyViewController->_beatmapSelectionView->gameObject->transform, this);
        if (segmentVert) _difficultyCanvasGroup = segmentVert->gameObject->AddComponent<UnityEngine::CanvasGroup*>();

        // PerPlayerToggles
        BSML::parse_and_construct(Assets::MpPerPlayerToggles_bsml, _lobbyViewController->_startGameReadyButton->gameObject->transform, this);

        // Check if UI Elements were created
        if (!segmentVert || !ppdt || !ppmt || !difficultyControl || !_difficultyCanvasGroup) {
            CRITICAL("Failed to create UI Elements, UI will not function!");
            return;
        }

        // Create Event Delegates
        didActivateDelegate = custom_types::MakeDelegate<HMUI::ViewController::DidActivateDelegate*>(this, (std::function<void(MultiplayerCore::UI::MpPerPlayerUI*, bool, bool, bool)>)&MpPerPlayerUI::DidActivate);
        didDeactivateDelegate = custom_types::MakeDelegate<HMUI::ViewController::DidDeactivateDelegate*>(this, (std::function<void(MultiplayerCore::UI::MpPerPlayerUI*, bool, bool)>)&MpPerPlayerUI::DidDeactivate);
        setLobbyStateDelegate = custom_types::MakeDelegate<System::Action_1<GlobalNamespace::MultiplayerLobbyState>*>(this, (std::function<void(MultiplayerCore::UI::MpPerPlayerUI*, GlobalNamespace::MultiplayerLobbyState)>)&MpPerPlayerUI::SetLobbyState);
        localSelectedBeatmapDelegate = custom_types::MakeDelegate<System::Action_1<GlobalNamespace::LevelSelectionFlowCoordinator::State*>*>(this, (std::function<void(MultiplayerCore::UI::MpPerPlayerUI*, GlobalNamespace::LevelSelectionFlowCoordinator::State*)>)&MpPerPlayerUI::LocalSelectedBeatmap);
        updateDifficultyListDelegate = custom_types::MakeDelegate<System::Action_1<GlobalNamespace::BeatmapKey>*>(this, (std::function<void(MultiplayerCore::UI::MpPerPlayerUI*, GlobalNamespace::BeatmapKey)>)&MpPerPlayerUI::UpdateDifficultyListWithBeatmapKey); // static_cast<void(MultiplayerCore::UI::MpPerPlayerUI::*)(GlobalNamespace::BeatmapKey)>()
        // Using std::bind to pass the class instance to the function, due to compilation problems when passing the class instance directly to custom_types::MakeDelegate
        clearLocalSelectedBeatmapDelegate = custom_types::MakeDelegate<System::Action*>(std::function<void()>(std::bind(&MpPerPlayerUI::ClearLocalSelectedBeatmap, this)));
        updateButtonsEnabledDelegate = custom_types::MakeDelegate<System::Action*>(std::function<void()>(std::bind(&MpPerPlayerUI::UpdateButtonsEnabled, this)));
        // clearLocalSelectedBeatmapDelegate = custom_types::MakeDelegate<System::Action*>(this, (std::function<void(MultiplayerCore::UI::MpPerPlayerUI*)>)&MpPerPlayerUI::ClearLocalSelectedBeatmap);
        // updateButtonsEnabledDelegate = custom_types::MakeDelegate<System::Action*>(this, (std::function<void(MultiplayerCore::UI::MpPerPlayerUI*)>)&MpPerPlayerUI::UpdateButtonsEnabled);

        // Add Event Listeners
        _lobbyViewController->add_didActivateEvent(didActivateDelegate);
        _lobbyViewController->add_didDeactivateEvent(didDeactivateDelegate);

        // Register MpPerPlayerPacket
        _packetSerializer->RegisterCallback<Players::Packets::MpPerPlayerPacket*>(std::bind(&MpPerPlayerUI::HandleMpPerPlayerPacket, this, std::placeholders::_1, std::placeholders::_2));
        _packetSerializer->RegisterCallback<Players::Packets::GetMpPerPlayerPacket*>(std::bind(&MpPerPlayerUI::HandleGetMpPerPlayerPacket, this, std::placeholders::_1, std::placeholders::_2));

        _gameStateController->add_lobbyStateChangedEvent(setLobbyStateDelegate);
        _gameServerLobbyFlowCoordinator->_multiplayerLevelSelectionFlowCoordinator->add_didSelectLevelEvent(localSelectedBeatmapDelegate);
        _gameServerLobbyFlowCoordinator->_serverPlayerListViewController->add_selectSuggestedBeatmapEvent(updateDifficultyListDelegate);
        _lobbyViewController->add_clearSuggestedBeatmapEvent(clearLocalSelectedBeatmapDelegate);
        _gameServerLobbyFlowCoordinator->_lobbyPlayerPermissionsModel->add_permissionsChangedEvent(updateButtonsEnabledDelegate);
    }

    void MpPerPlayerUI::Dispose() {
        // Dispose
        // Remove Event Listeners
        _lobbyViewController->remove_didActivateEvent(didActivateDelegate);
        _lobbyViewController->remove_didDeactivateEvent(didDeactivateDelegate);
        _gameStateController->remove_lobbyStateChangedEvent(setLobbyStateDelegate);
        _gameServerLobbyFlowCoordinator->_multiplayerLevelSelectionFlowCoordinator->remove_didSelectLevelEvent(localSelectedBeatmapDelegate);
        _gameServerLobbyFlowCoordinator->_serverPlayerListViewController->remove_selectSuggestedBeatmapEvent(updateDifficultyListDelegate);
        _lobbyViewController->remove_clearSuggestedBeatmapEvent(clearLocalSelectedBeatmapDelegate);
        _gameServerLobbyFlowCoordinator->_lobbyPlayerPermissionsModel->remove_permissionsChangedEvent(updateButtonsEnabledDelegate);

        // Unregister MpPerPlayerPacket
        _packetSerializer->UnregisterCallback<Players::Packets::MpPerPlayerPacket*>();
        _packetSerializer->UnregisterCallback<Players::Packets::GetMpPerPlayerPacket*>();
    }

    void MpPerPlayerUI::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
        // DidActivate
        if (firstActivation) {
            auto cgubPos = _lobbyViewController->_cancelGameUnreadyButton->transform->position;
            cgubPos.y -= 0.2f;
            _lobbyViewController->_cancelGameUnreadyButton->transform->position = cgubPos;

            auto sgrbPos = _lobbyViewController->_startGameReadyButton->transform->position;
            sgrbPos.y -= 0.2f;
            _lobbyViewController->_startGameReadyButton->transform->position = sgrbPos;

            auto csgHHPos = _lobbyViewController->_cantStartGameHoverHint->transform->position;
            csgHHPos.y -= 0.2f;
            _lobbyViewController->_cantStartGameHoverHint->transform->position = csgHHPos;
        }
        else
        {
            // Update Difficulty List
            auto lobbyPlayersDataModel = il2cpp_utils::try_cast<GlobalNamespace::LobbyPlayersDataModel>(_gameServerLobbyFlowCoordinator->_lobbyPlayersDataModel);
            if (lobbyPlayersDataModel) {
                bool _;
                GlobalNamespace::LobbyPlayerData* playerData = lobbyPlayersDataModel.value()->GetOrCreateLobbyPlayerDataModel(lobbyPlayersDataModel.value()->localUserId, byref(_));
                if (playerData) {
                    UpdateDifficultyListWithBeatmapKey(playerData->beatmapKey);
                }
            }

            if (addedToHierarchy)
            {
                // Reset our buttons
                ppdt->set_Value(false);
                ppmt->set_Value(false);

                // Request Updated state
                _multiplayerSessionManager->Send(Players::Packets::GetMpPerPlayerPacket::New_ctor());
            }
        }

        if (_lobbyViewController && _lobbyViewController->_isPartyOwner && ppdt && ppmt)
        {
            ppdt->gameObject->SetActive(true);
            ppmt->gameObject->SetActive(true);
        } else if (ppdt && ppmt) {
            ppdt->gameObject->SetActive(false);
            ppmt->gameObject->SetActive(false);
        } else ERROR("Toggle Settings are null, returning...");
    }

    void MpPerPlayerUI::DidDeactivate(bool removedFromHierarchy, bool screenSystemDisabling) {
        // DidDeactivate
        if (removedFromHierarchy) {
            
        }
    }


    void MpPerPlayerUI::HandleMpPerPlayerPacket(Players::Packets::MpPerPlayerPacket* packet, GlobalNamespace::IConnectedPlayer* player) {
        // HandleMpPerPlayerPacket
        DEBUG("Received MpPerPlayerPacket from {}|{} with values PPDEnabled: {} PPMEnabled: {}", player->userName, player->userId, packet->PPDEnabled, packet->PPMEnabled);
        
        // Check if player is party Owner
        if (ppdt && ppmt && player->get_isConnectionOwner() && (packet->PPDEnabled != ppdt->get_Value() || packet->PPMEnabled != ppmt->get_Value())) {
            DEBUG("Player is Connection Owner, updating toogle values");
            ppdt->set_Value(packet->PPDEnabled);
            ppmt->set_Value(packet->PPMEnabled);
        } else if (!player->get_isConnectionOwner()) {
            WARNING("Player is not Connection Owner, ignoring packet");
        }
    }

    void MpPerPlayerUI::HandleGetMpPerPlayerPacket(Players::Packets::GetMpPerPlayerPacket* packet, GlobalNamespace::IConnectedPlayer* player) {
        // HandleGetMpPerPlayerPacket
        DEBUG("Received GetMpPerPlayerPacket from {}|{}", player->userName, player->userId);
        // Send MpPerPlayerPacket
        auto ppPacket = Players::Packets::MpPerPlayerPacket::New_ctor();
        ppPacket->PPDEnabled = ppdt->get_Value();
        ppPacket->PPMEnabled = ppmt->get_Value();
        _multiplayerSessionManager->Send(ppPacket);
    }

    void MpPerPlayerUI::UpdateButtonsEnabled() {
        // UpdateButtonsEnabled
        if (_lobbyViewController && _lobbyViewController->_isPartyOwner && ppdt && ppmt)
        {
            ppdt->gameObject->SetActive(true);
            ppmt->gameObject->SetActive(true);

            _multiplayerSessionManager->Send(Players::Packets::GetMpPerPlayerPacket::New_ctor());
        } else if (ppdt && ppmt) {
            ppdt->gameObject->SetActive(false);
            ppmt->gameObject->SetActive(false);
        } else ERROR("Toggle Settings are null, returning...");
    }

    void MpPerPlayerUI::SetLobbyState(GlobalNamespace::MultiplayerLobbyState state) {
        // SetLobbyState
        DEBUG("Current Lobby State {}", Utils::EnumUtils::GetEnumName<GlobalNamespace::MultiplayerLobbyState>(state));
        if (_difficultyCanvasGroup) {
            _difficultyCanvasGroup->alpha = (state == GlobalNamespace::MultiplayerLobbyState::LobbySetup || 
                                             state == GlobalNamespace::MultiplayerLobbyState::LobbyCountdown) ? 1.0f : 0.25f;
        }

        auto cells = ListW<::UnityW<::HMUI::TableCell>>(difficultyControl->cells);
        for (auto& cell : cells) {
            if (!cell) continue;
            cell->set_interactable(state == GlobalNamespace::MultiplayerLobbyState::LobbySetup || 
                                   state == GlobalNamespace::MultiplayerLobbyState::LobbyCountdown);
        }

        if (!_lobbyViewController)
            return;

        if (_lobbyViewController->_isPartyOwner)
        {
            if (ppdt && ppmt) {
                ppdt->set_interactable(state == GlobalNamespace::MultiplayerLobbyState::LobbySetup || 
                                      state == GlobalNamespace::MultiplayerLobbyState::LobbyCountdown);
                ppmt->set_interactable(state == GlobalNamespace::MultiplayerLobbyState::LobbySetup ||
                                      state == GlobalNamespace::MultiplayerLobbyState::LobbyCountdown);
            }
        }
    }

    void MpPerPlayerUI::LocalSelectedBeatmap(GlobalNamespace::LevelSelectionFlowCoordinator::State* state) {
        // LocalSelectedBeatmap
        if (!state)
        {
            ERROR("LevelSelectionFlowCoordinator::State is null, returning...");
            return;
        }
        _currentBeatmapKey = state->beatmapKey;
        UpdateDifficultyListWithBeatmapKey(_currentBeatmapKey);
    }

    void MpPerPlayerUI::ClearLocalSelectedBeatmap() {
        // ClearLocalSelectedBeatmap
        if (segmentVert) {
            segmentVert->gameObject->SetActive(false);
        }
        _currentBeatmapKey = GlobalNamespace::BeatmapKey();
    }


    void MpPerPlayerUI::UpdateDifficultyListWithBeatmapKey(GlobalNamespace::BeatmapKey beatmapKey) {
        // UpdateDifficultyListWithBeatmapKey
        _currentBeatmapKey = beatmapKey;
        if (!_currentBeatmapKey.IsValid()) {
            DEBUG("Selected BeatmapKey invalid, disabling difficulty selector");
            if (segmentVert) segmentVert->gameObject->SetActive(false);
            else ERROR("segmentVert is null, returning...");
            return;
        }

        auto levelHash = HashFromLevelID(_currentBeatmapKey.levelId);
        if (!levelHash.empty() && _beatmapLevelProvider) {
            DEBUG("Level is custom, trying to get beatmap for hash {}", levelHash);
            std::shared_future fut = _beatmapLevelProvider->GetBeatmapAsync(levelHash);
            BSML::MainThreadScheduler::AwaitFuture(fut, [fut, levelHash, this](){
                auto levelOpt = il2cpp_utils::try_cast<MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel>(fut.get());
                if (!levelOpt.has_value()) {
                    ERROR("Failed to get level for hash {}", levelHash);
                    return;
                }

                auto level = levelOpt.value();
                if (level->requirements.empty()) {
                    ERROR("Level {} has empty requirements, this should not happen, falling back to packet", levelHash);
                    // Fallback to getting packet
                    level = il2cpp_utils::try_cast<MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel>(_beatmapLevelProvider->TryGetBeatmapFromPacketHash(levelHash)).value_or(nullptr);
                    if (!level) {
                        ERROR("Failed to get level from packet for hash {}", levelHash);
                        return;
                    }
                    if (level->requirements.empty()) {
                        ERROR("Level packet {} also has empty requirements, this should not happen...", levelHash);
                        return;
                    }
                }
                auto reqCharItr = level->requirements.find(_currentBeatmapKey.beatmapCharacteristic->serializedName);
                if (reqCharItr == level->requirements.end()) {
                    ERROR("Level {} failed to get requirements for characteristic {}", levelHash, _currentBeatmapKey.beatmapCharacteristic->serializedName);
                    return;
                }
                DEBUG("Got level {} Requirements for characteristic {} count {}", levelHash, _currentBeatmapKey.beatmapCharacteristic->serializedName, reqCharItr->second.size());
                // Hacky we use requirements to get the available difficulties
                ListW<StringW> difficulties = ListW<StringW>::New(reqCharItr->second.size());
                for (const auto& [key, value] : reqCharItr->second) {
                    difficulties->Add(Utils::EnumUtils::GetEnumName<GlobalNamespace::BeatmapDifficulty>(key));
                }
                // std::transform(reqCharItr->second.begin(), reqCharItr->second.end(), difficulties.begin(), [](const auto& pair) {
                //     return Utils::EnumUtils::GetEnumName<GlobalNamespace::BeatmapDifficulty>(pair.first);
                // });
                UpdateDifficultyList(difficulties);
            });
        }
        else
        {
            DEBUG("LevelId not custom: {}, getting difficulties from basegame", beatmapKey.levelId);
            auto level = _beatmapLevelsModel->GetBeatmapLevel(beatmapKey.levelId);
            if (level)
            {
                // Convert our difficulties into a string list
                auto enumerator = level->GetDifficulties(beatmapKey.beatmapCharacteristic)->GetEnumerator();
                ListW<StringW> difficulties = ListW<StringW>::New();
                while (enumerator->i___System__Collections__IEnumerator()->MoveNext())
                {
                    auto diffName = Utils::EnumUtils::GetEnumName<GlobalNamespace::BeatmapDifficulty>(enumerator->Current.value__)->Replace("ExpertPlus", "Expert+");
                    DEBUG("Adding difficulty {}", diffName);
                    difficulties->Add(diffName);
                }
                UpdateDifficultyList(difficulties);
            }
        }
    }

    void MpPerPlayerUI::UpdateDifficultyList(ListW<StringW> difficulties) {
        // UpdateDifficultyList
        if (!difficulties || !_allowedDifficulties || !_gameServerLobbyFlowCoordinator || !_gameServerLobbyFlowCoordinator->_unifiedNetworkPlayerModel) {
            ERROR("Values not set, returning...");
            return;
        }

        _allowedDifficulties->Clear();
        // Cast because cordl is missing most of the getters on the interface type
        auto upm = il2cpp_utils::try_cast<GlobalNamespace::UnifiedNetworkPlayerModel>(_gameServerLobbyFlowCoordinator->_unifiedNetworkPlayerModel);
        if (!upm.has_value()) {
            ERROR("Failed to get UnifiedNetworkPlayerModel, returning...");
            return;
        }
        auto diffMask = upm.value()->selectionMask.difficulties;
        for (auto& diff : difficulties) {
            // DEBUG("Checking if difficulty {} is in selection mask", diff);
            if (GlobalNamespace::BeatmapDifficultyMaskExtensions::Contains(diffMask, Utils::EnumUtils::GetEnumValue<GlobalNamespace::BeatmapDifficulty>(diff))) {
                DEBUG("Allowed difficulty {}", diff);
                _allowedDifficulties->Add(diff->Replace("ExpertPlus", "Expert+"));
            }
        }

        if (!segmentVert || !difficultyControl) {
            ERROR("UI Elements not created, returning...");
            return;
        }

        // Updating UI has to be done on the main thread
        BSML::MainThreadScheduler::Schedule([this](){
            if (_allowedDifficulties->Count > 1) {
                segmentVert->gameObject->SetActive(true);
                difficultyControl->SetTexts(_allowedDifficulties->i___System__Collections__Generic__IReadOnlyList_1_T_());
                int index = _allowedDifficulties->IndexOf(Utils::EnumUtils::GetEnumName(_currentBeatmapKey.difficulty)->Replace("ExpertPlus", "Expert+"));
                if (index >= 0) {
                    difficultyControl->SelectCellWithNumber(index);
                }
            } else {
                DEBUG("Less than 2 Difficulties available disabling selector");
                segmentVert->gameObject->SetActive(false);
            }
        });
    }

    void MpPerPlayerUI::SetSelectedDifficulty(HMUI::SegmentedControl* control, int index)
    {
        // SetSelectedDifficulty
        auto diffName = _allowedDifficulties->get_Item(index);
        auto diff = Utils::EnumUtils::GetEnumValue<GlobalNamespace::BeatmapDifficulty>(diffName->Replace("Expert+", "ExpertPlus"));
        if (diff.value__ == -1) {
            ERROR("Failed to get difficulty value for {}", diffName);
            return;
        }

        DEBUG("Selected difficulty at index {} - {}", index, diffName);
        _currentBeatmapKey.difficulty = diff;
        _gameServerLobbyFlowCoordinator->_lobbyPlayersDataModel->SetLocalPlayerBeatmapLevel(_currentBeatmapKey);
    }

    void MpPerPlayerUI::set_PerPlayerDifficulty(bool value) {
        // set_PerPlayerDifficulty
        if (ppdt) ppdt->set_Value(value);
        // Send updated MpPerPlayerPacket
        auto ppPacket = Players::Packets::MpPerPlayerPacket::New_ctor();
        ppPacket->PPDEnabled = value;
        if (ppmt) ppPacket->PPMEnabled = ppmt->get_Value();
        _multiplayerSessionManager->Send(ppPacket->i_INetSerializable());
    }

    bool MpPerPlayerUI::get_PerPlayerDifficulty() {
        // get_PerPlayerDifficulty
        return ppdt ? ppdt->get_Value() : false;
    }

    void MpPerPlayerUI::set_PerPlayerModifiers(bool value) {
        // set_PerPlayerModifiers
        if (ppmt) ppmt->set_Value(value);
        // Send updated MpPerPlayerPacket
        auto ppPacket = Players::Packets::MpPerPlayerPacket::New_ctor();
        if (ppdt) ppPacket->PPDEnabled = ppdt->get_Value();
        ppPacket->PPMEnabled = value;
        _multiplayerSessionManager->Send(ppPacket->i_INetSerializable());
    }

    bool MpPerPlayerUI::get_PerPlayerModifiers() {
        // get_PerPlayerModifiers
        return ppmt ? ppmt->get_Value() : false;
    }
}