#include "UI/CustomBeatmapSelectionView.hpp"
#include "Beatmaps/Packets/MpBeatmapPacket.hpp"
#include "Beatmaps/Providers/MpBeatmapLevelProvider.hpp"
#include "GlobalNamespace/LevelBar.hpp"
#include "GlobalNamespace/BeatmapBasicData.hpp"
#include "GlobalNamespace/EnvironmentName.hpp"
#include "UI/MpRequirementsUI.hpp"
#include "UnityEngine/GameObject.hpp"
#include "Utilities.hpp"
#include "logging.hpp"

DEFINE_TYPE(MultiplayerCore::UI, CustomBeatmapSelectionView);

namespace MultiplayerCore::UI {

    void CustomBeatmapSelectionView::Inject(Objects::MpPlayersDataModel* mpPlayersDataModel, Beatmaps::Providers::MpBeatmapLevelProvider* mpBeatmapLevelProvider) {
        _mpPlayersDataModel = mpPlayersDataModel;
        _mpBeatmapLevelProvider = mpBeatmapLevelProvider;
    }

    void CustomBeatmapSelectionView::SetBeatmap(GlobalNamespace::BeatmapKey beatmapKey) {
        static auto method = il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::BeatmapSelectionView::SetBeatmap>::methodInfo();
        static auto base = [](GlobalNamespace::BeatmapSelectionView* self, GlobalNamespace::BeatmapKey beatmapKey){ return il2cpp_utils::RunMethodRethrow<void, false>(self, method, beatmapKey); };
        if (!beatmapKey.IsValid()) {
            WARNING("beatmapkey was invalid, {} {} {}", beatmapKey.levelId, (int)beatmapKey.difficulty, beatmapKey.beatmapCharacteristic ? beatmapKey.beatmapCharacteristic->get_serializedName() : "null");
            return base(this, beatmapKey);
        }

        auto levelHash = HashFromLevelID(beatmapKey.levelId);
        if (levelHash.empty()) { // level wasn't custom
            DEBUG("Level {} wasn't custom", beatmapKey.levelId);
            return base(this, beatmapKey);
        }

        auto packet = _mpPlayersDataModel->FindLevelPacket(levelHash);
        if (!packet) { // level was probably selected by the local player from their installed maps, since no other player had it selected
            DEBUG("Could not find packet for levelhash {}, possibly local selection", levelHash);
            return base(this, beatmapKey);
        }

        // we have a packet to select from, update values correctly without calling base, and setup the levelbar with our own level
        _noLevelText->enabled = false;
        _levelBar->hide = false;

        // create a beatmap level for display
        auto mpLevel = _mpBeatmapLevelProvider->GetBeatmapFromPacket(packet);
        auto dict = System::Collections::Generic::Dictionary_2<System::ValueTuple_2<UnityW<GlobalNamespace::BeatmapCharacteristicSO>, GlobalNamespace::BeatmapDifficulty>, GlobalNamespace::BeatmapBasicData*>::New_ctor();
        dict->Add(
            System::ValueTuple_2<UnityW<GlobalNamespace::BeatmapCharacteristicSO>, GlobalNamespace::BeatmapDifficulty>(
                beatmapKey.beatmapCharacteristic,
                beatmapKey.difficulty
            ),
            GlobalNamespace::BeatmapBasicData::New_ctor(
                0, 0, GlobalNamespace::EnvironmentName::getStaticF_Empty(),
                nullptr, 0, 0, 0,
                {packet->levelAuthorName}, ArrayW<StringW>::Empty()
            )
        );

        mpLevel->beatmapBasicData = dict->i___System__Collections__Generic__IReadOnlyDictionary_2_TKey_TValue_();
        _levelBar->SetupData(mpLevel, beatmapKey.difficulty, beatmapKey.beatmapCharacteristic);
    }
}
