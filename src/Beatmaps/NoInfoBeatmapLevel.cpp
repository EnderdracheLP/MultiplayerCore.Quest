#include "Beatmaps/NoInfoBeatmapLevel.hpp"
#include "Beatmaps/BeatSaverPreviewMediaData.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/ValueTuple_2.hpp"

DEFINE_TYPE(MultiplayerCore::Beatmaps, NoInfoBeatmapLevel);

namespace MultiplayerCore::Beatmaps {
    void NoInfoBeatmapLevel::ctor_1(StringW hash) {
        INVOKE_CTOR();
        INVOKE_BASE_CTOR(classof(Abstractions::MpBeatmapLevel*));

        set_levelHash(static_cast<std::string>(hash));
        static ConstString Empty("");

        _ctor(
            false,
            fmt::format("custom_level_{}", hash),
            Empty,
            Empty,
            Empty,
            ArrayW<StringW>::Empty(),
            ArrayW<StringW>::Empty(),
            0,
            0,
            0,
            0,
            0,
            0,
            GlobalNamespace::PlayerSensitivityFlag::Safe,
            BeatSaverPreviewMediaData::New_ctor(hash)->i_IPreviewMediaData(),
            System::Collections::Generic::Dictionary_2<System::ValueTuple_2<UnityW<GlobalNamespace::BeatmapCharacteristicSO>, GlobalNamespace::BeatmapDifficulty>, GlobalNamespace::BeatmapBasicData*>::New_ctor()->i___System__Collections__Generic__IReadOnlyDictionary_2_TKey_TValue_()
        );
    }
}
