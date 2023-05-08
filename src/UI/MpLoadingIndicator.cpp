#include "UI/MpLoadingIndicator.hpp"
#include "logging.hpp"
#include "assets.hpp"

#include "bsml/shared/BSML.hpp"

#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Generic/IReadOnlyDictionary_2.hpp"
#include "GlobalNamespace/PreviewDifficultyBeatmap.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/ILobbyPlayerData.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Resources.hpp"

DEFINE_TYPE(MultiplayerCore::UI, MpLoadingIndicator);

template<typename T, typename U>
int IReadOnlyDictionary_2_Count(System::Collections::Generic::IReadOnlyDictionary_2<T, U>* dict) {
    auto coll = dict->i_KeyValuePair_2_TKey_TValue();
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
        BSML::parse_and_construct(IncludedAssets::LoadingIndicator_bsml, _screenController->get_transform(), this);
        auto existingLoadingControl = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::LoadingControl*>().First()->get_gameObject();
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
            _levelLoader->gameplaySetupData != nullptr
        ) {
            // FIXME: this will currently never find the entitlement for the local player, and it will always find an "extra" player because of the server being registered as one, this should be fixed
            int okCount = OkPlayerCountNoRequest();
            int totalCount = IReadOnlyDictionary_2_Count(_playersDataModel->i_ILobbyPlayerData());
            _loadingControl->ShowLoading(fmt::format("{} of {} players ready...", okCount, totalCount));
        } else {
            _loadingControl->Hide();
        }
    }

    int MpLoadingIndicator::OkPlayerCountNoRequest() {
        auto levelId = _levelLoader->gameplaySetupData->get_beatmapLevel()->get_beatmapLevel()->get_levelID();

        auto dict = _playersDataModel->i_ILobbyPlayerData();
        auto coll = dict->i_KeyValuePair_2_TKey_TValue();
        auto enumerable = coll->i_IEnumerable_1_T();
        auto enumerator_1 = enumerable->GetEnumerator();
        auto enumerator = enumerator_1->i_IEnumerator();

        int okCount = 0;
        auto loggerContext = getLogger().WithContext("OkPlayerCountNoRequest");
        auto get_current_minfo = il2cpp_utils::FindMethodUnsafe(reinterpret_cast<Il2CppObject*>(enumerator)->klass, "System.Collections.Generic.IEnumerator<System.Collections.Generic.KeyValuePair<System.String,ILobbyPlayerData>>.get_Current", 0);
        using KVP = System::Collections::Generic::KeyValuePair_2<StringW, GlobalNamespace::ILobbyPlayerData *>;
        while (enumerator->MoveNext()) {
            auto cur = RET_0_UNLESS(loggerContext, il2cpp_utils::RunMethod<KVP>(enumerator, get_current_minfo));

            auto tocheck = cur.key;
            if (_entitlementChecker->GetUserEntitlementStatusWithoutRequest(tocheck, levelId) == GlobalNamespace::EntitlementsStatus::Ok) okCount++;
        }

        il2cpp_utils::RunMethod(enumerator_1, "System.IDisposable.Dispose");
        return okCount;
    }

    void MpLoadingIndicator::Report(double value) {
        _downloadProgress = value;
        _isDownloading = value < 1.0;
    }
}
