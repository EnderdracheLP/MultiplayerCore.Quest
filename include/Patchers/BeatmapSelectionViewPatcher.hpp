#pragma once

#include "custom-types/shared/macros.hpp"
#include "System/IDisposable.hpp"
#include "Zenject/IInitializable.hpp"
#include "GlobalNamespace/LobbySetupViewController.hpp"
#include "GlobalNamespace/EditableBeatmapSelectionView.hpp"
#include "Zenject/DiContainer.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerCore::Patchers, BeatmapSelectionViewPatcher, System::Object, std::vector<Il2CppClass*>({classof(Zenject::IInitializable*), classof(System::IDisposable*)}),
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::LobbySetupViewController*, _lobbySetupViewController);
    DECLARE_INSTANCE_FIELD_PRIVATE(Zenject::DiContainer*, _container);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::EditableBeatmapSelectionView*, _oldBeatmapSelectionView);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::EditableBeatmapSelectionView*, _customBeatmapSelectionView);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
    DECLARE_CTOR(ctor, GlobalNamespace::LobbySetupViewController* lobbySetupViewController, Zenject::DiContainer* container);
)
