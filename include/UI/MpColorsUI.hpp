#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include "bsml/shared/macros.hpp"

#include "GlobalNamespace/LobbySetupViewController.hpp"
#include "GlobalNamespace/ColorSchemeView.hpp"
#include "Beatmaps/Abstractions/DifficultyColors.hpp"
#include "bsml/shared/BSML/Components/ModalView.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/Vector3.hpp"

DECLARE_CLASS_CODEGEN(MultiplayerCore::UI, MpColorsUI, System::Object) {
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::LobbySetupViewController*, _lobbySetupViewController);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::ColorSchemeView*, _colorSchemeView);

    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::ModalView*, modal);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Vector3, modalPosition);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::RectTransform*, selectedColorTransform);
    DECLARE_BSML_PROPERTY(bool, colors);

    DECLARE_INSTANCE_METHOD(void, PostParse);
    DECLARE_CTOR(ctor, GlobalNamespace::LobbySetupViewController* lobbySetupViewController);

    public:
        UnorderedEventCallback<> dismissedEvent;

        void ShowColors();
        void Parse();
        void Dismiss();

        void AcceptColors(Beatmaps::Abstractions::DifficultyColors const& colors);
        void AcceptColors(SongCore::CustomJSONData::CustomSaveDataInfo::BasicCustomDifficultyBeatmapDetails::CustomColors const& colors);
};
