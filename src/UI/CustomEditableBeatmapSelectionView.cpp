#include "UI/CustomEditableBeatmapSelectionView.hpp"
#include "Beatmaps/Packets/MpBeatmapPacket.hpp"
#include "Beatmaps/Providers/MpBeatmapLevelProvider.hpp"
#include "GlobalNamespace/LevelBar.hpp"
#include "UI/MpRequirementsUI.hpp"
#include "UnityEngine/GameObject.hpp"
#include "Utilities.hpp"
#include "logging.hpp"

DEFINE_TYPE(MultiplayerCore::UI, CustomEditableBeatmapSelectionView);

namespace MultiplayerCore::UI {

    void CustomEditableBeatmapSelectionView::Inject(Objects::MpPlayersDataModel* mpPlayersDataModel, Beatmaps::Providers::MpBeatmapLevelProvider* mpBeatmapLevelProvider) {
        _mpPlayersDataModel = mpPlayersDataModel;
        _mpBeatmapLevelProvider = mpBeatmapLevelProvider;
    }

    void CustomEditableBeatmapSelectionView::SetBeatmap(ByRef<GlobalNamespace::BeatmapKey> beatmapKey) {
        static auto method = il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::EditableBeatmapSelectionView::SetBeatmap>::methodInfo();
        static auto base = [](GlobalNamespace::EditableBeatmapSelectionView* self, ByRef<GlobalNamespace::BeatmapKey> beatmapKey){ return il2cpp_utils::RunMethodRethrow<void, false>(self, method, beatmapKey); };
        if (!beatmapKey->IsValid()) {
            DEBUG("beatmapkey was invalid");
            return base(this, beatmapKey);
        }

        auto levelHash = HashFromLevelID(beatmapKey->levelId);
        if (levelHash.empty()) { // level wasn't custom
            DEBUG("Level {} wasn't custom", beatmapKey->levelId);
            return base(this, beatmapKey);
        }

        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(SetBeatmapCoroutine(beatmapKey.heldRef, levelHash)));
    }

    custom_types::Helpers::Coroutine CustomEditableBeatmapSelectionView::SetBeatmapCoroutine(GlobalNamespace::BeatmapKey beatmapKey, std::string levelHash) {
        auto packet = _mpPlayersDataModel->FindLevelPacket(levelHash);
        GlobalNamespace::BeatmapLevel* level = nullptr;
        std::shared_future<GlobalNamespace::BeatmapLevel*> fut;
        if (!packet) { // level was probably selected by the local player from their installed maps, since no other player had it selected
            DEBUG("Could not find packet for levelhash {}, falling back to beatsaver", levelHash);
            fut = _mpBeatmapLevelProvider->GetBeatmapAsync(levelHash);
            while (fut.valid() && fut.wait_for(std::chrono::seconds(0)) != std::future_status::ready) co_yield nullptr;
            level = fut.get();
        } else {
            // create a beatmap level for display
            level = _mpBeatmapLevelProvider->GetBeatmapFromPacket(packet);
        }

        if (!level) {
            WARNING("Couldn't get level, returning...");
            co_return;
        }
        
        // we have a packet to select from, update values correctly without calling base, and setup the levelbar with our own level
        _clearButton->gameObject->SetActive(showClearButton);
        _noLevelText->enabled = false;
        _levelBar->hide = false;


        level = _mpBeatmapLevelProvider->AddBasicBeatmapDataToLevel(level, beatmapKey, packet);

        _levelBar->Setup(level, beatmapKey.difficulty, beatmapKey.beatmapCharacteristic);
        co_return;
    }
}
