#include "UI/MpColorsUI.hpp"

#include "logging.hpp"
#include "assets.hpp"
#include "bsml/shared/BSML.hpp"
#include "GlobalNamespace/LevelBar.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/Resources.hpp"

DEFINE_TYPE(MultiplayerCore::UI, MpColorsUI);

namespace MultiplayerCore::UI {
    void MpColorsUI::ctor(GlobalNamespace::LobbySetupViewController* lobbySetupViewController) {
        INVOKE_CTOR();

        _lobbySetupViewController = lobbySetupViewController;
    }

    void MpColorsUI::PostParse() {
        _colorSchemeView = UnityEngine::Object::Instantiate(
            UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::ColorSchemeView*>()->First(),
            selectedColorTransform
        );

        modalPosition = modal->get_transform()->get_localPosition();
        modal->onHide = [this]{ this->dismissedEvent.invoke(); };
    }

    void MpColorsUI::ShowColors(const Beatmaps::Abstractions::DifficultyColors& colors) {
        Parse();
        modal->Show();
        SetColors(colors);
    }

    void MpColorsUI::Parse() {
        if (!modal || !modal->m_CachedPtr) {
            BSML::parse_and_construct(
                Assets::ColorsUI_bsml,
                _lobbySetupViewController->GetComponentInChildren<GlobalNamespace::LevelBar*>()->get_transform(),
                this
            );
        }

        modal->get_transform()->set_localPosition(modalPosition);
    }

    void MpColorsUI::Dismiss() {
        modal->Hide();
    }

    void MpColorsUI::SetColors(const Beatmaps::Abstractions::DifficultyColors& colors) {
        static MultiplayerCore::Utils::ExtraSongData::MapColor voidColor = MultiplayerCore::Utils::ExtraSongData::MapColor(0.5f, 0.5f, 0.5f);

        _colorSchemeView->SetColors(
            colors.colorLeft.value_or(voidColor),
            colors.colorRight.value_or(voidColor),
            colors.envColorLeft.value_or(voidColor),
            colors.envColorRight.value_or(voidColor),
            colors.envColorLeftBoost.value_or(voidColor),
            colors.envColorRightBoost.value_or(voidColor),
            colors.obstacleColor.value_or(voidColor)
        );
    }

    bool MpColorsUI::get_colors() { return true; }
    void MpColorsUI::set_colors(bool value) {}
}
