#include "Objects/MpLevelLoader.hpp"
#include "Utils/ExtraSongData.hpp"
#include "lapiz/shared/utilities/MainThreadScheduler.hpp"
#include "songcore/shared/SongLoader/RuntimeSongLoader.hpp"
#include "logging.hpp"
#include "tasks.hpp"

#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapLevelsModel.hpp"
#include "System/Threading/CancellationTokenSource.hpp"

DEFINE_TYPE(MultiplayerCore::Objects, MpLevelLoader);

namespace MultiplayerCore::Objects {
    void MpLevelLoader::ctor(SongCore::SongLoader::RuntimeSongLoader* runtimeSongLoader, SongCore::Capabilities* capabilities, GlobalNamespace::IMultiplayerSessionManager* sessionManager, MpLevelDownloader* levelDownloader, GlobalNamespace::NetworkPlayerEntitlementChecker* entitlementChecker, GlobalNamespace::IMenuRpcManager* rpcManager) {
        INVOKE_CTOR();
        INVOKE_BASE_CTOR(classof(GlobalNamespace::MultiplayerLevelLoader*));

        _runtimeSongLoader = runtimeSongLoader;
        _capabilities = capabilities;
        _sessionManager = sessionManager;
        _levelDownloader = levelDownloader;
        _entitlementChecker = il2cpp_utils::try_cast<MpEntitlementChecker>(entitlementChecker).value_or(nullptr);
        _rpcManager = rpcManager;
    }

    void MpLevelLoader::LoadLevel_override(GlobalNamespace::ILevelGameplaySetupData* gameplaySetupData, long initialStartTime) {
        auto key = gameplaySetupData->beatmapKey;
        std::string levelId(key.levelId);
        std::string levelHash(!levelId.empty() ? SongCore::SongLoader::RuntimeSongLoader::GetHashFromLevelID(levelId) : "");

        DEBUG("Loading Level '{}'", levelHash.empty() ? levelId : levelHash);
        LoadLevel(gameplaySetupData, initialStartTime);
        if (!levelHash.empty() && !_runtimeSongLoader->GetLevelByHash(levelHash))
            _getBeatmapLevelResultTask = StartDownloadBeatmapLevelAsyncTask(levelId, _getBeatmapCancellationTokenSource->Token);
    }

    void MpLevelLoader::Tick_override() {
        using MultiplayerBeatmapLoaderState = GlobalNamespace::MultiplayerLevelLoader::MultiplayerBeatmapLoaderState;

        auto levelId = _gameplaySetupData ? _gameplaySetupData->beatmapKey.levelId : nullptr;

        if (!levelId || System::String::IsNullOrEmpty(levelId)) {
            GlobalNamespace::MultiplayerLevelLoader::Tick();
            return;
        }

        auto beatmapKey = _gameplaySetupData->beatmapKey;

        switch (_loaderState) {
            case MultiplayerBeatmapLoaderState::LoadingBeatmap: {
                GlobalNamespace::MultiplayerLevelLoader::Tick();
                if (_loaderState == MultiplayerBeatmapLoaderState::WaitingForCountdown) {
                    _rpcManager->SetIsEntitledToLevel(levelId, GlobalNamespace::EntitlementsStatus::Ok);
                    DEBUG("Loaded level {}", levelId);
                    auto customLevel = SongCore::API::Loading::GetLevelByLevelID(static_cast<std::string>(levelId));
                    if (!customLevel) break;

                    auto saveData = customLevel->standardLevelInfoSaveData;
                    if (!saveData) break;

                    auto diffDetailsOpt = saveData->TryGetCharacteristicAndDifficulty(beatmapKey.beatmapCharacteristic->serializedName, beatmapKey.difficulty);
                    if (!diffDetailsOpt.has_value()) break;

                    auto& diffDetails = diffDetailsOpt->get();
                    for (const auto& req : diffDetails.requirements) {
                        if (!_capabilities->IsCapabilityRegistered(req)) {
                            _rpcManager->SetIsEntitledToLevel(levelId, GlobalNamespace::EntitlementsStatus::NotOwned);
                            break;
                        }
                    }
                }
            } break;
            case MultiplayerBeatmapLoaderState::WaitingForCountdown: {
                if (_sessionManager->get_syncTime() >= _startTime) {
                    bool allFinished = true;
                    int pCount = _sessionManager->get_connectedPlayerCount();
                    for (std::size_t i = 0; i < pCount; i++) {
                        auto p = _sessionManager->GetConnectedPlayer(i);
                        bool hasEntitlement = _entitlementChecker->GetUserEntitlementStatusWithoutRequest(p->get_userId(), levelId) == GlobalNamespace::EntitlementsStatus::Ok;
                        bool in_gameplay = p->HasState("in_gameplay");

                        if (!(hasEntitlement || in_gameplay)) {
                            allFinished = false;
                            break;
                        }
                    }

                    if (allFinished) {
                        DEBUG("All players finished loading");
                        GlobalNamespace::MultiplayerLevelLoader::Tick();
                    }
                }
            } break;
            default:
                GlobalNamespace::MultiplayerLevelLoader::Tick();
                break;
        }
    }

    System::Threading::Tasks::Task_1<GlobalNamespace::LoadBeatmapLevelDataResult>* MpLevelLoader::StartDownloadBeatmapLevelAsyncTask(std::string levelId, System::Threading::CancellationToken cancellationToken) {
        return StartTask<GlobalNamespace::LoadBeatmapLevelDataResult>([this, levelId, cancellationToken](CancellationToken token) -> GlobalNamespace::LoadBeatmapLevelDataResult {
            static auto Error = GlobalNamespace::LoadBeatmapLevelDataResult(true, nullptr);
            _levelDownloader->TryDownloadLevelAsync(levelId, std::bind(&MpLevelLoader::Report, this, std::placeholders::_1)).wait();
            auto level = _runtimeSongLoader->GetLevelByLevelID(levelId);
            if (!level) return Error;
            if (!level->beatmapLevelData) return Error;
            return GlobalNamespace::LoadBeatmapLevelDataResult::Success(level->beatmapLevelData);
        }, std::forward<CancellationToken>(cancellationToken));
    }

    void MpLevelLoader::Report(double progress) {
        progressUpdated.invoke(progress);
    }
}
