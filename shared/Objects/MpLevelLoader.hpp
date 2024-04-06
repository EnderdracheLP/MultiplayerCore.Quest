#pragma once

#include "custom-types/shared/macros.hpp"
#include "MpLevelDownloader.hpp"
#include "MpEntitlementChecker.hpp"
#include "songcore/shared/Capabilities.hpp"
#include "songcore/shared/SongLoader/RuntimeSongLoader.hpp"

#include "GlobalNamespace/MultiplayerLevelLoader.hpp"
#include "GlobalNamespace/NetworkPlayerEntitlementChecker.hpp"
#include "GlobalNamespace/IMultiplayerSessionManager.hpp"
#include "GlobalNamespace/ILevelGameplaySetupData.hpp"
#include "GlobalNamespace/IMenuRpcManager.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Threading/CancellationToken.hpp"

#include "Zenject/ITickable.hpp"
#include <cstdarg>

DECLARE_CLASS_CODEGEN(MultiplayerCore::Objects, MpLevelLoader, GlobalNamespace::MultiplayerLevelLoader,
    DECLARE_INSTANCE_FIELD_PRIVATE(SongCore::SongLoader::RuntimeSongLoader*, _runtimeSongLoader);
    DECLARE_INSTANCE_FIELD_PRIVATE(SongCore::Capabilities*, _capabilities);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::IMultiplayerSessionManager*, _sessionManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(MpLevelDownloader*, _levelDownloader);
    DECLARE_INSTANCE_FIELD_PRIVATE(MpEntitlementChecker*, _entitlementChecker);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::IMenuRpcManager*, _rpcManager);

    DECLARE_INSTANCE_METHOD(void, LoadLevel_override, GlobalNamespace::ILevelGameplaySetupData* gameplaySetupData, long initialStartTime);
    DECLARE_INSTANCE_METHOD(void, Tick_override);
    DECLARE_CTOR(ctor, SongCore::SongLoader::RuntimeSongLoader* runtimeSongLoader, SongCore::Capabilities* capabilities, GlobalNamespace::IMultiplayerSessionManager* sessionManager, MpLevelDownloader* levelDownloader, GlobalNamespace::NetworkPlayerEntitlementChecker* entitlementChecker, GlobalNamespace::IMenuRpcManager* rpcManager);

    public:
        System::Threading::Tasks::Task_1<GlobalNamespace::LoadBeatmapLevelDataResult>* StartDownloadBeatmapLevelAsyncTask(std::string levelId, System::Threading::CancellationToken cancellationToken);
        UnorderedEventCallback<double> progressUpdated;
    private:
        void UnloadLevelIfRequirementsNotMet();
        void Report(double progress);
)
