#pragma once

#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "lapiz/shared/macros.hpp"

#include "Objects/MpPlayersDataModel.hpp"
#include "UI/MpRequirementsUI.hpp"
#include "Beatmaps/Providers/MpBeatmapLevelProvider.hpp"
#include "GlobalNamespace/EditableBeatmapSelectionView.hpp"

DECLARE_CLASS_CODEGEN(MultiplayerCore::UI, CustomBeatmapSelectionView, GlobalNamespace::BeatmapSelectionView,
    DECLARE_INSTANCE_FIELD_PRIVATE(Objects::MpPlayersDataModel*, _mpPlayersDataModel);
    DECLARE_INSTANCE_FIELD_PRIVATE(Beatmaps::Providers::MpBeatmapLevelProvider*, _mpBeatmapLevelProvider);

    DECLARE_OVERRIDE_METHOD_MATCH(void, SetBeatmap, &GlobalNamespace::BeatmapSelectionView::SetBeatmap, ByRef<GlobalNamespace::BeatmapKey> beatmapKey);

    DECLARE_INJECT_METHOD(void, Inject, MultiplayerCore::Objects::MpPlayersDataModel* mpPlayersDataModel, Beatmaps::Providers::MpBeatmapLevelProvider* mpBeatmapLevelProvider);

    public:
    custom_types::Helpers::Coroutine SetBeatmapCoroutine(GlobalNamespace::BeatmapKey beatmapKey, std::string levelHash);
)
