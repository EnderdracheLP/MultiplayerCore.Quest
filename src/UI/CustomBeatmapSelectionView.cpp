#include "UI/CustomBeatmapSelectionView.hpp"
#include "Beatmaps/Packets/MpBeatmapPacket.hpp"
#include "Beatmaps/Providers/MpBeatmapLevelProvider.hpp"
#include "GlobalNamespace/LevelBar.hpp"
#include "UI/MpRequirementsUI.hpp"
#include "UnityEngine/GameObject.hpp"
#include "Utilities.hpp"

DEFINE_TYPE(MultiplayerCore::UI, CustomBeatmapSelectionView);

namespace MultiplayerCore::UI {

    void CustomBeatmapSelectionView::Inject(Objects::MpPlayersDataModel* mpPlayersDataModel, Beatmaps::Providers::MpBeatmapLevelProvider* mpBeatmapLevelProvider) {
        _mpPlayersDataModel = mpPlayersDataModel;
        _mpBeatmapLevelProvider = mpBeatmapLevelProvider;
    }

    void CustomBeatmapSelectionView::SetBeatmap(ByRef<GlobalNamespace::BeatmapKey> beatmapKey) {
        static auto method = il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::EditableBeatmapSelectionView::SetBeatmap>::methodInfo();
        static auto base = [](GlobalNamespace::EditableBeatmapSelectionView* self, ByRef<GlobalNamespace::BeatmapKey> beatmapKey){ return il2cpp_utils::RunMethodRethrow<void, false>(self, method, beatmapKey); };
        if (!beatmapKey->IsValid()) return base(this, beatmapKey);

        auto levelHash = HashFromLevelID(beatmapKey->levelId);
        if (levelHash.empty()) return base(this, beatmapKey); // level wasn't custom

        auto packet = _mpPlayersDataModel->FindLevelPacket(levelHash);
        if (!packet) return base(this, beatmapKey); // level was probably selected by the local player from their installed maps, since no other player had it selected

        // we have a packet to select from, update values correctly without calling base, and setup the levelbar with our own level
        _clearButton->gameObject->SetActive(showClearButton);
        _noLevelText->enabled = false;
        _levelBar->hide = false;

        // create a beatmap level for display
        auto level = _mpBeatmapLevelProvider->GetBeatmapFromPacket(packet);
        _levelBar->Setup(level, beatmapKey->beatmapCharacteristic, beatmapKey->difficulty);
    }
}
