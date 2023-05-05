#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include "bsml/shared/macros.hpp"

#include "GlobalNamespace/LobbySetupViewController.hpp"
#include "GlobalNamespace/ColorSchemeView.hpp"
#include "Beatmaps/Abstractions/DifficultyColors.hpp"
#include "bsml/shared/BSML/Components/ModalView.hpp"
#include "UnityEngine/RectTransform.hpp"

// TODO: fields
DECLARE_CLASS_CODEGEN(MultiplayerCore::UI, MpColorsUI, Il2CppObject,
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

        void ShowColors(const Beatmaps::Abstractions::DifficultyColors& colors);
        void Parse();
        void Dismiss();
        void SetColors(const Beatmaps::Abstractions::DifficultyColors& colors);
)
