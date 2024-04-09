#include "Patchers/BeatmapSelectionViewPatcher.hpp"
#include "UI/CustomBeatmapSelectionView.hpp"
#include "UnityEngine/GameObject.hpp"

DEFINE_TYPE(MultiplayerCore::Patchers, BeatmapSelectionViewPatcher);

namespace MultiplayerCore::Patchers {
    void BeatmapSelectionViewPatcher::ctor(GlobalNamespace::LobbySetupViewController* lobbySetupViewController, Zenject::DiContainer* container) {
        _lobbySetupViewController = lobbySetupViewController;
        _container = container;
    }

    void BeatmapSelectionViewPatcher::Initialize() {
        // replace game selection view with ours
        _oldBeatmapSelectionView = _lobbySetupViewController->_beatmapSelectionView;
        _customBeatmapSelectionView = _container->InstantiateComponent<MultiplayerCore::UI::CustomBeatmapSelectionView*>(_oldBeatmapSelectionView->gameObject);
        auto offset = sizeof(UnityEngine::MonoBehaviour);
        auto sz = sizeof(GlobalNamespace::EditableBeatmapSelectionView) - offset;
        // copy all data over apart from the base monobehaviour data, this way everything is ensured to be correct
        std::memcpy(((char*)_customBeatmapSelectionView + offset), ((char*)_oldBeatmapSelectionView + offset), sz);
        _lobbySetupViewController->_beatmapSelectionView = _customBeatmapSelectionView;
    }

    void BeatmapSelectionViewPatcher::Dispose() {
        // turn things back to how they were
        _lobbySetupViewController->_beatmapSelectionView = _oldBeatmapSelectionView;
        UnityEngine::Object::Destroy(_customBeatmapSelectionView);
    }
}
