#include "UI/MpLoadingIndicator.hpp"
#include "logging.hpp"
#include "assets.hpp"

#include "bsml/shared/BSML.hpp"

#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Generic/IReadOnlyDictionary_2.hpp"
#include "System/Collections/Generic/KeyValuePair_2.hpp"
#include "System/Collections/Generic/IReadOnlyCollection_1.hpp"
#include "System/Collections/Generic/IEnumerable_1.hpp"
#include "System/Collections/Generic/IEnumerator_1.hpp"
#include "GlobalNamespace/ILobbyPlayerData.hpp"
#include "GlobalNamespace/ILobbyPlayersDataModel.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Resources.hpp"

DEFINE_TYPE(MultiplayerCore::UI, MpLoadingIndicator);

int ILobbyPlayersDataModel_Count(GlobalNamespace::ILobbyPlayersDataModel* model) {
    using namespace System::Collections::Generic;
    auto dict = static_cast<IReadOnlyDictionary_2<::StringW, ::GlobalNamespace::ILobbyPlayerData*>*>(*model);
    auto coll = static_cast<IReadOnlyCollection_1<KeyValuePair_2<::StringW, ::GlobalNamespace::ILobbyPlayerData*>>*>(*dict);
    return coll->get_Count();
}

namespace MultiplayerCore::UI {
    void MpLoadingIndicator::ctor(
        GlobalNamespace::IMultiplayerSessionManager* sessionManager,
        GlobalNamespace::ILobbyGameStateController* gameStateController,
        GlobalNamespace::ILobbyPlayersDataModel* playersDataModel,
        GlobalNamespace::NetworkPlayerEntitlementChecker* entitlementChecker,
        Objects::MpLevelLoader* levelLoader,
        GlobalNamespace::CenterStageScreenController* screenController
    ) {
        INVOKE_CTOR();

        _sessionManager = sessionManager;
        _gameStateController = gameStateController;
        _playersDataModel = playersDataModel;
        _entitlementChecker = il2cpp_utils::try_cast<Objects::MpEntitlementChecker>(entitlementChecker).value_or(nullptr);
        _levelLoader = levelLoader;
        _screenController = screenController;
    }

    void MpLoadingIndicator::Dispose() {
        _levelLoader->progressUpdated -= {&MpLoadingIndicator::Report, this};
    }

    void MpLoadingIndicator::Initialize() {
        BSML::parse_and_construct(Assets::LoadingIndicator_bsml, _screenController->get_transform(), this);
        auto existingLoadingControl = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::LoadingControl*>()->First()->gameObject;
        auto go = UnityEngine::Object::Instantiate(existingLoadingControl, vert->get_transform());
        _loadingControl = go->GetComponent<GlobalNamespace::LoadingControl*>();
        _loadingControl->Hide();

        _levelLoader->progressUpdated += {&MpLoadingIndicator::Report, this};
    }

    void MpLoadingIndicator::Tick() {
        if (_isDownloading) {
            _loadingControl->ShowDownloadingProgress(fmt::format("Downloading ({:.2f}%)...", (float)_downloadProgress), _downloadProgress);
            return;
        } else if (
            _screenController->get_countdownShown() &&
            _sessionManager->get_syncTime() >= _gameStateController->get_startTime() &&
            _gameStateController->get_levelStartInitiated() &&
            _levelLoader->_gameplaySetupData != nullptr
        ) {
            int okCount = OkPlayerCountNoRequest();
            // We subtract 1 since we don't count the server
            int totalCount = ILobbyPlayersDataModel_Count(_playersDataModel) - 1;
            _loadingControl->ShowLoading(fmt::format("{} of {} players ready...", okCount, totalCount));
        } else {
            _loadingControl->Hide();
        }
    }

    int MpLoadingIndicator::OkPlayerCountNoRequest() {
        using namespace System::Collections;
        using namespace System::Collections::Generic;
        auto key = _levelLoader->_gameplaySetupData->beatmapKey;
        auto levelId = key.levelId;

        auto& dict = *_playersDataModel;
        auto enumerable = static_cast<IEnumerable_1<KeyValuePair_2<::StringW, ::GlobalNamespace::ILobbyPlayerData*>>*>(dict);
        auto enumerator_1 = enumerable->GetEnumerator();
        auto enumerator = static_cast<IEnumerator*>(*enumerator_1);

        // Starts at 1 because the local player is already checked at this point
        int okCount = 1;
        while (enumerator->MoveNext()) {
            auto cur = enumerator_1->Current;
            auto tocheck = cur.key;
            if (_entitlementChecker->GetUserEntitlementStatusWithoutRequest(tocheck, levelId) == GlobalNamespace::EntitlementsStatus::Ok) okCount++;
        }

        enumerator_1->i___System__IDisposable()->Dispose();
        return okCount;
    }

    void MpLoadingIndicator::Report(double value) {
        _downloadProgress = value;
        _isDownloading = value < 1.0;
    }
}
