#include "Patchers/BeatmapSelectionViewPatcher.hpp"
#include "UI/CustomEditableBeatmapSelectionView.hpp"
#include "UI/CustomBeatmapSelectionView.hpp"
#include "UnityEngine/GameObject.hpp"

DEFINE_TYPE(MultiplayerCore::Patchers, BeatmapSelectionViewPatcher);

namespace MultiplayerCore::Patchers {
    void BeatmapSelectionViewPatcher::ctor(GlobalNamespace::LobbySetupViewController* lobbySetupViewController, GlobalNamespace::CenterStageScreenController* centerStageScreenController, Zenject::DiContainer* container) {
        _lobbySetupViewController = lobbySetupViewController;
        _centerStageScreenController = centerStageScreenController;
        _container = container;
    }

    void BeatmapSelectionViewPatcher::Initialize() {
        {
            // replace game selection view with ours
            _oldLobbyBeatmapSelectionView = _lobbySetupViewController->_beatmapSelectionView;
            _customLobbyBeatmapSelectionView = _container->InstantiateComponent<MultiplayerCore::UI::CustomEditableBeatmapSelectionView*>(_oldLobbyBeatmapSelectionView->gameObject);
            auto offset = sizeof(UnityEngine::MonoBehaviour);
            auto sz = sizeof(GlobalNamespace::EditableBeatmapSelectionView) - offset;
            // copy all data over apart from the base monobehaviour data, this way everything is ensured to be correct
            std::memcpy(((char*)_customLobbyBeatmapSelectionView + offset), ((char*)_oldLobbyBeatmapSelectionView + offset), sz);
            _lobbySetupViewController->_beatmapSelectionView = _customLobbyBeatmapSelectionView;
        }

        {
            _oldCenterBeatmapSelectionView = _centerStageScreenController->_beatmapSelectionView;
            _customCenterBeatmapSelectionView = _container->InstantiateComponent<MultiplayerCore::UI::CustomBeatmapSelectionView*>(_oldCenterBeatmapSelectionView->gameObject);
            auto offset = sizeof(UnityEngine::MonoBehaviour);
            auto sz = sizeof(GlobalNamespace::BeatmapSelectionView) - offset;
            // copy all data over apart from the base monobehaviour data, this way everything is ensured to be correct
            std::memcpy(((char*)_customCenterBeatmapSelectionView + offset), ((char*)_oldCenterBeatmapSelectionView + offset), sz);
            _centerStageScreenController->_beatmapSelectionView = _customCenterBeatmapSelectionView;
        }
    }

    void BeatmapSelectionViewPatcher::Dispose() {
        // turn things back to how they were
        _lobbySetupViewController->_beatmapSelectionView = _oldLobbyBeatmapSelectionView;
        UnityEngine::Object::Destroy(_customLobbyBeatmapSelectionView);
        _centerStageScreenController->_beatmapSelectionView = _oldCenterBeatmapSelectionView;
        UnityEngine::Object::Destroy(_customCenterBeatmapSelectionView);
    }
}
