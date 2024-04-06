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
    return model->i___System__Collections__Generic__IReadOnlyCollection_1___System__Collections__Generic__KeyValuePair_2___StringW___GlobalNamespace__ILobbyPlayerData___()->Count;
    // auto dict = static_cast<IReadOnlyDictionary_2<::StringW, ::GlobalNamespace::ILobbyPlayerData*>*>(*model);
    // auto coll = static_cast<IReadOnlyCollection_1<KeyValuePair_2<::StringW, ::GlobalNamespace::ILobbyPlayerData*>>*>(*dict);
    // return coll->get_Count();
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
            auto [okCount, totalCount] = OkAndTotalPlayerCountNoRequest();
            // We subtract 1 since we don't count the server
            totalCount--;
            _loadingControl->ShowLoading(fmt::format("{} of {} players ready...", okCount, totalCount));
        } else {
            _loadingControl->Hide();
        }
    }

    std::pair<int, int> MpLoadingIndicator::OkAndTotalPlayerCountNoRequest() {
        using namespace System::Collections;
        using namespace System::Collections::Generic;
        auto key = _levelLoader->_gameplaySetupData->beatmapKey;
        auto levelId = key.levelId;

        auto enumerable = _playersDataModel->i___System__Collections__Generic__IEnumerable_1___System__Collections__Generic__KeyValuePair_2___StringW___GlobalNamespace__ILobbyPlayerData___();
        auto enumerator_1 = enumerable->GetEnumerator();
        auto enumerator = enumerator_1->i___System__Collections__IEnumerator();

        // Starts at 1 because the local player is already checked at this point
        int okCount = 0;
        int totalCount = 0;
        while (enumerator->MoveNext()) {
            totalCount++;
            auto [userId, playerData] = enumerator_1->Current;
            auto knownEntitlement = _entitlementChecker->GetKnownEntitlement(userId, levelId);
            DEBUG("User: {}, entitlement OK: {}", userId, knownEntitlement == GlobalNamespace::EntitlementsStatus::Ok);
            if (knownEntitlement == GlobalNamespace::EntitlementsStatus::Ok) okCount++;
        }

        enumerator_1->i___System__IDisposable()->Dispose();
        return {okCount, totalCount};
    }

    void MpLoadingIndicator::Report(double value) {
        _downloadProgress = value;
        _isDownloading = value < 1.0;
    }
}
