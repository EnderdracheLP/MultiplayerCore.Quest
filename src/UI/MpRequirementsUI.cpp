#include "UI/MpRequirementsUI.hpp"
#include "Beatmaps/Abstractions/MpBeatmapLevel.hpp"
#include "Beatmaps/BeatSaverBeatmapLevel.hpp"
#include "logging.hpp"
#include "assets.hpp"

#include "custom-types/shared/delegate.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/utilities.hpp"

#include "GlobalNamespace/EditableBeatmapSelectionView.hpp"
#include "GlobalNamespace/LevelBar.hpp"
#include "GlobalNamespace/ILobbyPlayerData.hpp"
#include "GlobalNamespace/PreviewDifficultyBeatmap.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "HMUI/TableView_ScrollPositionType.hpp"

DEFINE_TYPE(MultiplayerCore::UI, MpRequirementsUI);

// Accessing "private" method from pinkcore
namespace RequirementUtils {
    bool GetRequirementInstalled(std::string requirement);
}

namespace MultiplayerCore::UI {
    void MpRequirementsUI::ctor(
            GlobalNamespace::LobbySetupViewController* lobbySetupViewController,
            GlobalNamespace::ILobbyPlayersDataModel* playersDataModel,
            MpColorsUI* colorsUI
    ) {
        INVOKE_CTOR();

        _lobbySetupViewController = lobbySetupViewController;
        _playersDataModel = playersDataModel;
        _colorsUI = colorsUI;
        data = List<BSML::CustomCellInfo*>::New_ctor();
    }

    void MpRequirementsUI::Initialize() {
        auto bar = _lobbySetupViewController->GetComponentInChildren<GlobalNamespace::EditableBeatmapSelectionView*>()->get_transform()->Find("LevelBarSimple")->GetComponent<GlobalNamespace::LevelBar*>();
        BSML::parse_and_construct(IncludedAssets::RequirementsButton_bsml, bar->get_transform(), this);
        auto buttonT = infoButton->get_transform();
        buttonT->set_localScale(buttonT->get_localScale() * 0.7f);
        buttonT->get_gameObject()->SetActive(true);
        _buttonBG = buttonT->Find("BG")->GetComponent<HMUI::ImageView*>();
        _originalColor0 = _buttonBG->get_color0();
        _originalColor1 = _buttonBG->get_color1();

        beatmapSelectedAction = custom_types::MakeDelegate<System::Action_1<StringW>*>(
            std::function<void(StringW)>(std::bind(&MpRequirementsUI::BeatmapSelected, this, std::placeholders::_1))
        );
        _playersDataModel->add_didChangeEvent(beatmapSelectedAction);
        _colorsUI->dismissedEvent += {&MpRequirementsUI::ColorsDismissed, this};
    }

    void MpRequirementsUI::Dispose() {
        _playersDataModel->remove_didChangeEvent(beatmapSelectedAction);
        _colorsUI->dismissedEvent -= {&MpRequirementsUI::ColorsDismissed, this};
    }

    void MpRequirementsUI::BeatmapSelected(StringW) {
        auto beatmapLevel = _playersDataModel->i_ILobbyPlayerData()->get_Item(_playersDataModel->get_localUserId())->i_ILevelGameplaySetupData()->get_beatmapLevel();
        auto mpLevel = beatmapLevel ? il2cpp_utils::try_cast<Beatmaps::Abstractions::MpBeatmapLevel>(beatmapLevel->get_beatmapLevel()).value_or(nullptr) : nullptr;
        if (mpLevel) {
            bool buttonShouldBeActive = false;
            std::string chName(beatmapLevel->get_beatmapCharacteristic()->get_name());
            std::string chSerName(beatmapLevel->get_beatmapCharacteristic()->get_serializedName());

            auto& diffColors = mpLevel->difficultyColors;
            auto colorMapItr = diffColors.find(chName);
            if (colorMapItr == diffColors.end()) colorMapItr = diffColors.find(chSerName);

            if (colorMapItr != diffColors.end()) {
                auto colorItr = colorMapItr->second.find(beatmapLevel->beatmapDifficulty.value);
                if (colorItr != colorMapItr->second.end()) {
                    buttonShouldBeActive = colorItr->second.AnyAreNotNull();
                }
            }

            auto reqMapItr = mpLevel->requirements.find(chName);
            if (reqMapItr == mpLevel->requirements.end()) reqMapItr = mpLevel->requirements.find(chSerName);

            if (reqMapItr != mpLevel->requirements.end()) {
                auto reqItr = reqMapItr->second.find(beatmapLevel->beatmapDifficulty.value);
                if (reqItr != reqMapItr->second.end()) {
                    buttonShouldBeActive = buttonShouldBeActive || !reqItr->second.empty();
                }
            }

            buttonShouldBeActive = buttonShouldBeActive || !mpLevel->contributors.empty();

            set_buttonInteractable(buttonShouldBeActive);
        } else {
            set_buttonInteractable(false);
        }
    }

    template<typename T, typename U>
    inline bool contains(T& collection, U& value) {
        return collection.find(value) != collection.end();
    }

    void MpRequirementsUI::ShowRequirements() {
        if (!modal || !modal->m_CachedPtr.m_value) {
            BSML::parse_and_construct(IncludedAssets::RequirementsUI_bsml, root, this);
            modalPosition = modal->get_transform()->get_localPosition();
        }

        modal->get_transform()->set_localPosition(modalPosition);
        modal->Show();
        data->Clear();

        auto localUserId = _playersDataModel->get_localUserId();
        GlobalNamespace::ILobbyPlayerData* localPlayerDataModel = nullptr;
        if (!_playersDataModel->i_ILobbyPlayerData()->TryGetValue(localUserId, byref(localPlayerDataModel))) {
            ERROR("Could not get local player info");
            return;
        }
        auto level = localPlayerDataModel->i_ILevelGameplaySetupData()->get_beatmapLevel();
        auto mpLevel = il2cpp_utils::try_cast<Beatmaps::Abstractions::MpBeatmapLevel>(level->get_beatmapLevel()).value_or(nullptr);

        if (mpLevel) {
            auto diffColorsMap = mpLevel->difficultyColors;
            auto reqsMap = mpLevel->requirements;
            auto diff = level->get_beatmapDifficulty();

            StringW chname;
            auto _chname = level->get_beatmapCharacteristic()->get_name();
            auto _schname = level->get_beatmapCharacteristic()->get_serializedName();

            if (contains(diffColorsMap, _chname) || contains(reqsMap, _chname)) chname = _chname;
            else if (contains(diffColorsMap, _schname) || contains(reqsMap, _schname)) chname = _schname;

            if (!chname) return;

            // requirements
            auto reqItr = reqsMap.find(chname);
            if (reqItr != reqsMap.end()) {
                auto reqsItr = reqItr->second.find(diff.value);
                if (reqsItr != reqItr->second.end()) {
                    for (const auto& req : reqsItr->second) {
                        auto installed = RequirementUtils::GetRequirementInstalled(req);
                        auto cell = BSML::CustomCellInfo::construct(
                            fmt::format("<size=75%>{}", req),
                            installed ? "Requirement" : "Missing Requirement",
                            installed ? get_HaveReqIcon() : get_MissingReqIcon()
                        );
                        data->Add(cell);
                    }
                } else {
                    ERROR("Issue finding diff {} to get requirements", diff.value);
                }
            } else {
                ERROR("Issue finding characteristic {} to get requirements", chname);
            }

            // contributors
            if (!mpLevel->contributors.empty()) {
                DEBUG("Adding contributors");
                for (const auto& contributor : mpLevel->contributors) {
                    // TODO: actually load the proper sprites, but skipping for now
                    data->Add(BSML::CustomCellInfo::construct(contributor.name, contributor.role, get_InfoIcon()));
                }
            } else {
                DEBUG("No Contributors found");
            }

            // colors
            auto colorMapItr = diffColorsMap.find(chname);
            BSML::CustomCellInfo* colorCell = nullptr;
            if (colorMapItr != diffColorsMap.end()) {
                auto colorsItr = colorMapItr->second.find(diff);
                if (colorsItr != colorMapItr->second.end()) {
                    if (colorsItr->second.AnyAreNotNull())
                        colorCell = BSML::CustomCellInfo::construct("<size=75%>Custom Colors Available", "Click here to preview it.", get_ColorsIcon());
                } else {
                    ERROR("Issue finding diff {} to get colors", diff.value);
                }
            } else {
                ERROR("Issue finding characteristic {} to get colors", chname);
            }

            if (!colorCell && il2cpp_utils::try_cast<Beatmaps::BeatSaverBeatmapLevel>(mpLevel).has_value())
                colorCell = BSML::CustomCellInfo::construct("<size=75%>Custom Colors", "Click here to preview it.", get_ColorsIcon());

            if (colorCell) data->Add(colorCell);

            DEBUG("There should be {} cells", data->get_Count());
            list->tableView->ReloadData();
            list->tableView->ScrollToCellWithIdx(0, HMUI::TableView::ScrollPositionType::Beginning, false);
        }
    }

    void MpRequirementsUI::Select(HMUI::TableView* tableView, int index) {
        auto localUserData = _playersDataModel->i_ILobbyPlayerData()->get_Item(_playersDataModel->get_localUserId());
        auto beatmapLevel = localUserData->i_ILevelGameplaySetupData()->get_beatmapLevel();
        auto mpLevel = il2cpp_utils::try_cast<Beatmaps::Abstractions::MpBeatmapLevel>(beatmapLevel->get_beatmapLevel()).value_or(nullptr);
        if (mpLevel) {
            auto diffColors = mpLevel->difficultyColors;

            auto colorMapItr = diffColors.find(beatmapLevel->get_beatmapCharacteristic()->get_name());
            if (colorMapItr == diffColors.end()) colorMapItr = diffColors.find(beatmapLevel->get_beatmapCharacteristic()->get_serializedName());

            list->tableView->ClearSelection();
            if (colorMapItr != diffColors.end()) {
                if (list->data[index]->icon == get_ColorsIcon()) {
                    auto colorItr = colorMapItr->second.find(beatmapLevel->beatmapDifficulty.value);
                    if (colorItr != colorMapItr->second.end()) {
                        modal->Hide();
                        _colorsUI->ShowColors(colorItr->second);
                    }
                }
            }
        }
    }

    void MpRequirementsUI::ColorsDismissed() { ShowRequirements(); }
    bool MpRequirementsUI::get_buttonGlowColor() { return _buttonGlowColor; }
    void MpRequirementsUI::set_buttonGlowColor(bool value) {
        _buttonGlowColor = value;
        infoButton->get_gameObject()->SetActive(true);
    }

    bool MpRequirementsUI::get_buttonInteractable() { return _buttonInteractable; }
    void MpRequirementsUI::set_buttonInteractable(bool value) {
        _buttonInteractable = value;
        infoButton->set_interactable(value);
    }

#define DEFINE_ICON(icon) \
    UnityEngine::Sprite* MpRequirementsUI::get_##icon##Icon() { \
        if (!_##icon##Icon || !_##icon##Icon->m_CachedPtr.m_value) { \
            auto& data = IncludedAssets::icon##_png;\
            _##icon##Icon = BSML::Utilities::LoadSpriteRaw(data);\
        }\
        return _##icon##Icon;\
    }

    DEFINE_ICON(HaveReq);
    DEFINE_ICON(MissingReq);
    DEFINE_ICON(HaveSuggestion);
    DEFINE_ICON(MissingSuggestion);
    DEFINE_ICON(Warning);
    DEFINE_ICON(Colors);
    DEFINE_ICON(Info);
    DEFINE_ICON(MissingSprite);
}
