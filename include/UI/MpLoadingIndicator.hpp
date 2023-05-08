#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "Objects/MpEntitlementChecker.hpp"
#include "Objects/MpLevelLoader.hpp"
#include "GlobalNamespace/IMultiplayerSessionManager.hpp"
#include "GlobalNamespace/ILobbyGameStateController.hpp"
#include "GlobalNamespace/ILobbyPlayersDataModel.hpp"
#include "GlobalNamespace/NetworkPlayerEntitlementChecker.hpp"
#include "GlobalNamespace/CenterStageScreenController.hpp"
#include "GlobalNamespace/LoadingControl.hpp"

#include "System/IDisposable.hpp"
#include "Zenject/IInitializable.hpp"
#include "Zenject/ITickable.hpp"
#include "UnityEngine/GameObject.hpp"

// TODO: fields
DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerCore::UI, MpLoadingIndicator, Il2CppObject,
    std::vector<Il2CppClass*>({classof(::System::IDisposable*), classof(::Zenject::IInitializable*), classof(::Zenject::ITickable*)}),

    DECLARE_INSTANCE_FIELD_PRIVATE(double, _downloadProgress);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::GameObject*, vert);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::IMultiplayerSessionManager*, _sessionManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::ILobbyGameStateController*, _gameStateController);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::ILobbyPlayersDataModel*, _playersDataModel);
    DECLARE_INSTANCE_FIELD_PRIVATE(Objects::MpEntitlementChecker*, _entitlementChecker);
    DECLARE_INSTANCE_FIELD_PRIVATE(Objects::MpLevelLoader*, _levelLoader);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::CenterStageScreenController*, _screenController);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::LoadingControl*, _loadingControl);

    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());
    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());
    DECLARE_OVERRIDE_METHOD(void, Tick, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::ITickable::Tick>::get());
    DECLARE_INSTANCE_METHOD(void, Report, double value);

    DECLARE_CTOR(ctor,
        GlobalNamespace::IMultiplayerSessionManager* sessionManager,
        GlobalNamespace::ILobbyGameStateController* gameStateController,
        GlobalNamespace::ILobbyPlayersDataModel* playersDataModel,
        GlobalNamespace::NetworkPlayerEntitlementChecker* entitlementChecker,
        Objects::MpLevelLoader* levelLoader,
        GlobalNamespace::CenterStageScreenController* screenController
    );

    public:
        int OkPlayerCountNoRequest();
        bool get_isDownloading();
)
