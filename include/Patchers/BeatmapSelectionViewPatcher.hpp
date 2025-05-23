#pragma once

#include "custom-types/shared/macros.hpp"
#include "System/IDisposable.hpp"
#include "Zenject/IInitializable.hpp"
#include "GlobalNamespace/LobbySetupViewController.hpp"
#include "GlobalNamespace/CenterStageScreenController.hpp"
#include "GlobalNamespace/EditableBeatmapSelectionView.hpp"
#include "Zenject/DiContainer.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerCore::Patchers, BeatmapSelectionViewPatcher, System::Object, Zenject::IInitializable*, System::IDisposable*) {
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::LobbySetupViewController*, _lobbySetupViewController);
    DECLARE_INSTANCE_FIELD_PRIVATE( GlobalNamespace::CenterStageScreenController*, _centerStageScreenController);
    DECLARE_INSTANCE_FIELD_PRIVATE(Zenject::DiContainer*, _container);

    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::EditableBeatmapSelectionView*, _oldLobbyBeatmapSelectionView);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::EditableBeatmapSelectionView*, _customLobbyBeatmapSelectionView);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::BeatmapSelectionView*, _oldCenterBeatmapSelectionView);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::BeatmapSelectionView*, _customCenterBeatmapSelectionView);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
    DECLARE_CTOR(ctor, GlobalNamespace::LobbySetupViewController* lobbySetupViewController, GlobalNamespace::CenterStageScreenController* centerStageScreenController, Zenject::DiContainer* container);
};