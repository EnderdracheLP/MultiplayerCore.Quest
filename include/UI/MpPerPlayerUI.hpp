#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "GlobalNamespace/GameServerLobbyFlowCoordinator.hpp"
#include "GlobalNamespace/BeatmapLevelsModel.hpp"


#include "System/Object.hpp"
#include "System/IDisposable.hpp"
#include "Zenject/IInitializable.hpp"

#include "UnityEngine/UI/VerticalLayoutGroup.hpp"
#include "UnityEngine/UI/HorizontalLayoutGroup.hpp"
#include "UnityEngine/CanvasGroup.hpp"

#include "HMUI/TextSegmentedControl.hpp"

#include "bsml/shared/BSML/Components/Settings/ToggleSetting.hpp"

#include "Beatmaps/Providers/MpBeatmapLevelProvider.hpp"
#include "Networking/MpPacketSerializer.hpp"

#include "Players/Packets/MpPerPlayerPacket.hpp"
#include "Players/Packets/GetMpPerPlayerPacket.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerCore::UI, MpPerPlayerUI, System::Object, std::vector<Il2CppClass*>({ classof(::Zenject::IInitializable*), classof(::System::IDisposable*)}),

    DECLARE_INSTANCE_FIELD(GlobalNamespace::GameServerLobbyFlowCoordinator*, _gameServerLobbyFlowCoordinator);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::LobbySetupViewController*, _lobbyViewController);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::ILobbyGameStateController*, _gameStateController);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::BeatmapLevelsModel*, _beatmapLevelsModel);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::MultiplayerSessionManager*, _multiplayerSessionManager);
    DECLARE_INSTANCE_FIELD(MultiplayerCore::Beatmaps::Providers::MpBeatmapLevelProvider*, _beatmapLevelProvider);
    DECLARE_INSTANCE_FIELD(MultiplayerCore::Networking::MpPacketSerializer*, _packetSerializer);

    // Values
    DECLARE_INSTANCE_FIELD(GlobalNamespace::BeatmapKey, _currentBeatmapKey);
    DECLARE_INSTANCE_FIELD(ListW<StringW>, _allowedDifficulties);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);

    DECLARE_INSTANCE_METHOD(void, DidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    DECLARE_INSTANCE_METHOD(void, ModifierSelectionDidActivate, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
    // DECLARE_INSTANCE_METHOD(void, DidDeactivate, bool removedFromHierarchy, bool screenSystemDisabling);
    DECLARE_INSTANCE_METHOD(void, SetLobbyState, GlobalNamespace::MultiplayerLobbyState state);
    DECLARE_INSTANCE_METHOD(void, LocalSelectedBeatmap, GlobalNamespace::LevelSelectionFlowCoordinator::State* state);
    DECLARE_INSTANCE_METHOD(void, UpdateDifficultyListWithBeatmapKey, GlobalNamespace::BeatmapKey beatmapKey);
    DECLARE_INSTANCE_METHOD(void, UpdateDifficultyList, ListW<StringW> difficulties);
    DECLARE_INSTANCE_METHOD(void, ClearLocalSelectedBeatmap);
    DECLARE_INSTANCE_METHOD(void, HandleMpPerPlayerPacket, MultiplayerCore::Players::Packets::MpPerPlayerPacket* packet, GlobalNamespace::IConnectedPlayer* player);
    DECLARE_INSTANCE_METHOD(void, HandleGetMpPerPlayerPacket, MultiplayerCore::Players::Packets::GetMpPerPlayerPacket* packet, GlobalNamespace::IConnectedPlayer* player);
    DECLARE_INSTANCE_METHOD(void, UpdateButtonsEnabled);

    DECLARE_INSTANCE_METHOD(void, SetSelectedDifficulty, HMUI::SegmentedControl* control, int index);
    DECLARE_INSTANCE_METHOD(void, set_PerPlayerDifficulty, bool value);
    DECLARE_INSTANCE_METHOD(bool, get_PerPlayerDifficulty);
    DECLARE_INSTANCE_METHOD(void, set_PerPlayerModifiers, bool value);
    DECLARE_INSTANCE_METHOD(bool, get_PerPlayerModifiers);

    DECLARE_CTOR(ctor,
            GlobalNamespace::GameServerLobbyFlowCoordinator* gameServerLobbyFlowCoordinator,
            GlobalNamespace::BeatmapLevelsModel* beatmapLevelsModel,
            GlobalNamespace::IMultiplayerSessionManager* sessionManager,
            MultiplayerCore::Beatmaps::Providers::MpBeatmapLevelProvider* beatmapLevelProvider,
            MultiplayerCore::Networking::MpPacketSerializer* packetSerializer
    );

    // UI Elements
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::VerticalLayoutGroup*, segmentVert);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::HorizontalLayoutGroup*, ppth);
    DECLARE_INSTANCE_FIELD(HMUI::TextSegmentedControl*, difficultyControl);
    DECLARE_INSTANCE_FIELD(BSML::ToggleSetting*, ppdt);
    DECLARE_INSTANCE_FIELD(BSML::ToggleSetting*, ppmt);
    DECLARE_INSTANCE_FIELD(UnityEngine::CanvasGroup*, _difficultyCanvasGroup);

    // Event Listeners
    // Delegates
    DECLARE_INSTANCE_FIELD(HMUI::ViewController::DidActivateDelegate*, didActivateDelegate);
    DECLARE_INSTANCE_FIELD(HMUI::ViewController::DidActivateDelegate*, modifierSelectionDidActivateDelegate);
    // DECLARE_INSTANCE_FIELD(HMUI::ViewController::DidDeactivateDelegate*, didDeactivateDelegate);
    DECLARE_INSTANCE_FIELD(System::Action_1<GlobalNamespace::MultiplayerLobbyState>*, setLobbyStateDelegate);
    DECLARE_INSTANCE_FIELD(System::Action_1<GlobalNamespace::LevelSelectionFlowCoordinator::State*>*, localSelectedBeatmapDelegate);
    DECLARE_INSTANCE_FIELD(System::Action_1<GlobalNamespace::BeatmapKey>*, updateDifficultyListDelegate);
    DECLARE_INSTANCE_FIELD(System::Action*, clearLocalSelectedBeatmapDelegate);
    DECLARE_INSTANCE_FIELD(System::Action*, updateButtonsEnabledDelegate);
)