#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "MpColorsUI.hpp"

#include "GlobalNamespace/LobbySetupViewController.hpp"
#include "GlobalNamespace/ILobbyPlayersDataModel.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "System/IDisposable.hpp"
#include "System/Action_1.hpp"
#include "Zenject/IInitializable.hpp"

#include "bsml/shared/BSML/Components/CustomListTableData.hpp"
#include "bsml/shared/BSML/Components/ModalView.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerCore::UI, MpRequirementsUI, System::Object,
    std::vector<Il2CppClass*>({classof(::System::IDisposable*), classof(::Zenject::IInitializable*)}),

    DECLARE_INSTANCE_FIELD_PRIVATE(System::Action_1<StringW>*, beatmapSelectedAction);

    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _HaveReqIcon);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _MissingReqIcon);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _HaveSuggestionIcon);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _MissingSuggestionIcon);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _WarningIcon);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _InfoIcon);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _ColorsIcon);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _MissingSpriteIcon);

    DECLARE_INSTANCE_METHOD(UnityEngine::Sprite*, get_HaveReqIcon);
    DECLARE_INSTANCE_METHOD(UnityEngine::Sprite*, get_MissingReqIcon);
    DECLARE_INSTANCE_METHOD(UnityEngine::Sprite*, get_HaveSuggestionIcon);
    DECLARE_INSTANCE_METHOD(UnityEngine::Sprite*, get_MissingSuggestionIcon);
    DECLARE_INSTANCE_METHOD(UnityEngine::Sprite*, get_WarningIcon);
    DECLARE_INSTANCE_METHOD(UnityEngine::Sprite*, get_InfoIcon);
    DECLARE_INSTANCE_METHOD(UnityEngine::Sprite*, get_ColorsIcon);
    DECLARE_INSTANCE_METHOD(UnityEngine::Sprite*, get_MissingSpriteIcon);

    DECLARE_INSTANCE_FIELD_PRIVATE(HMUI::ImageView*, _buttonBG);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Color, _originalColor0);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Color, _originalColor1);

    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::LobbySetupViewController*, _lobbySetupViewController);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::ILobbyPlayersDataModel*, _playersDataModel);
    DECLARE_INSTANCE_FIELD_PRIVATE(MpColorsUI*, _colorsUI);

    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::CustomListTableData*, list);
    DECLARE_BSML_PROPERTY(bool, buttonGlowColor);
    DECLARE_BSML_PROPERTY(bool, buttonInteractable);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Vector3, modalPosition);
    DECLARE_INSTANCE_FIELD_PRIVATE(BSML::ModalView*, modal);
    DECLARE_INSTANCE_FIELD_PRIVATE(ListW<BSML::CustomCellInfo*>, data);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::UI::Button*, infoButton);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Transform*, root);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);

    DECLARE_CTOR(ctor,
            GlobalNamespace::LobbySetupViewController* lobbySetupViewController,
            GlobalNamespace::ILobbyPlayersDataModel* playersDataModel,
            MpColorsUI* colorsUI
    );

    DECLARE_INSTANCE_METHOD(void, Select, HMUI::TableView* tableView, int index);
    DECLARE_INSTANCE_METHOD(void, ShowRequirements);

    private:
        void BeatmapSelected(StringW);
        void ColorsDismissed();
)
