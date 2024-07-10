#include "UI/MpRequirementsUI.hpp"
#include "Beatmaps/Abstractions/MpBeatmapLevel.hpp"
#include "Beatmaps/BeatSaverBeatmapLevel.hpp"
#include "Objects/MpPlayersDataModel.hpp"
#include "Utilities.hpp"
#include "logging.hpp"
#include "assets.hpp"

#include "custom-types/shared/delegate.hpp"
#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/utilities.hpp"

#include "System/Collections/Generic/IReadOnlyDictionary_2.hpp"
#include "GlobalNamespace/EditableBeatmapSelectionView.hpp"
#include "GlobalNamespace/LevelBar.hpp"
#include "GlobalNamespace/ILobbyPlayerData.hpp"
#include "GlobalNamespace/ILevelGameplaySetupData.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"

DEFINE_TYPE(MultiplayerCore::UI, MpRequirementsUI);

static constexpr inline UnityEngine::Vector3 operator*(UnityEngine::Vector3 vec, float val) {
    return {
        vec.x * val,
        vec.y * val,
        vec.z * val,
    };
}

namespace MultiplayerCore::UI {
    void MpRequirementsUI::ctor(
            SongCore::SongLoader::RuntimeSongLoader* runtimeSongLoader,
            SongCore::Capabilities* capabilities,
            SongCore::PlayButtonInteractable* playButtonInteractable,
            GlobalNamespace::BeatmapLevelsModel* beatmapLevelsModel,
            GlobalNamespace::LobbySetupViewController* lobbySetupViewController,
            Objects::MpPlayersDataModel* playersDataModel,
            MpColorsUI* colorsUI
    ) {
        INVOKE_CTOR();
        _runtimeSongLoader = runtimeSongLoader;
        _capabilities = capabilities;
        _playButtonInteractable = playButtonInteractable;
        _beatmapLevelsModel = beatmapLevelsModel;
        _lobbySetupViewController = lobbySetupViewController;
        _playersDataModel = playersDataModel;
        _colorsUI = colorsUI;

        _data = ListW<BSML::CustomCellInfo*>::New();
        _levelInfoCells = ListW<BSML::CustomCellInfo*>::New();
        _disablingModsCells = ListW<BSML::CustomCellInfo*>::New();
        _unusedCells = ListW<BSML::CustomCellInfo*>::New();
    }

    void MpRequirementsUI::Initialize() {
        auto bar = _lobbySetupViewController->_beatmapSelectionView->_levelBar;
        BSML::parse_and_construct(Assets::RequirementsButton_bsml, bar->transform, this);
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

        _playButtonInteractable->PlayButtonDisablingModsChanged += {&MpRequirementsUI::PlayButtonDisablingModsChanged, this};

        BSML::parse_and_construct(Assets::RequirementsUI_bsml, root, this);
        modalPosition = modal->get_transform()->get_localPosition();
    }

    void MpRequirementsUI::Dispose() {
        _playersDataModel->remove_didChangeEvent(beatmapSelectedAction);
        _colorsUI->dismissedEvent -= {&MpRequirementsUI::ColorsDismissed, this};
        _playButtonInteractable->PlayButtonDisablingModsChanged -= {&MpRequirementsUI::PlayButtonDisablingModsChanged, this};
    }

    void MpRequirementsUI::BeatmapSelected(StringW) {
        using namespace System::Collections::Generic;

        auto localUserId = _playersDataModel->localUserId;
        auto playerDict = static_cast<IReadOnlyDictionary_2<StringW, GlobalNamespace::ILobbyPlayerData*>*>(*_playersDataModel);
        auto playerData = playerDict->Item[localUserId];

        auto key = playerData->i___GlobalNamespace__ILevelGameplaySetupData()->beatmapKey;
        // if our key isn't valid we can't do anything
        if (!key.IsValid()) return;

        auto levelId = key.levelId;
        auto localLevel = _beatmapLevelsModel->GetBeatmapLevel(levelId);
        if (localLevel) { // we got a level to set reqs from
            SetRequirementsFromLevel(localLevel, key);
            return;
        }

        auto levelHash = HashFromLevelID(levelId);
        auto packet = _playersDataModel->FindLevelPacket(levelHash);
        if (packet) { // we have a packet to set reqs from
            SetRequirementsFromPacket(packet);
            return;
        }

        SetNoRequirementsFound(); // nothing found to set from
    }

    template<typename T, typename U>
    inline bool contains(T& collection, U& value) {
        return collection.find(value) != collection.end();
    }

    void MpRequirementsUI::ShowRequirements() {
        using namespace System::Collections::Generic;
        modal->get_transform()->set_localPosition(modalPosition);
        modal->Show();
    }

    void MpRequirementsUI::Select(HMUI::TableView* tableView, int index) {
        using namespace System::Collections::Generic;
        auto cell = _data[index];
        if (cell->icon == get_ColorsIcon()) { // colors icon pressed
            _colorsUI->ShowColors();
        }

        tableView->ClearSelection();
    }

    void MpRequirementsUI::UpdateRequirementButton() {
        set_buttonInteractable(!_data.empty());
    }

    BSML::CustomCellInfo* MpRequirementsUI::GetCellInfo() {
        // if we had none, return a new one
        if (_unusedCells.empty()) return BSML::CustomCellInfo::New_ctor();
        auto lastIdx = _unusedCells.size() - 1;
        auto cell = _unusedCells[lastIdx];
        _unusedCells->Remove(cell);
        if (!cell) {
            DEBUG("Cell was nullptr, returning new one");
            return BSML::CustomCellInfo::New_ctor();
        }
        return cell;
    }

    void MpRequirementsUI::UpdateDataCells() {
        _data.clear();

        for (auto cell : _levelInfoCells) _data.push_back(cell);
        for (auto cell : _disablingModsCells) _data.push_back(cell);

        list->tableView->ReloadData();
        list->tableView->ScrollToCellWithIdx(0, HMUI::TableView::ScrollPositionType::Beginning, false);

        UpdateRequirementButton();
    }

    void MpRequirementsUI::ClearCells(ListW<BSML::CustomCellInfo*> toClear) {
        for (auto cell : toClear) _unusedCells.push_back(cell);
        toClear.clear();
    }

    void MpRequirementsUI::ColorsDismissed() { ShowRequirements(); }

    void MpRequirementsUI::SetRequirementsFromLevel(GlobalNamespace::BeatmapLevel* level, GlobalNamespace::BeatmapKey& beatmapKey) {
        ClearCells(_levelInfoCells);

        auto customLevel = il2cpp_utils::try_cast<SongCore::SongLoader::CustomBeatmapLevel>(level).value_or(nullptr);
        // hacky way of being able to just break out of an "if" via break rather than a goto: label
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch-bool"
        switch (customLevel != nullptr) {
#pragma clang diagnostic pop
            // non-null ptr
            case true: {
                auto levelDetailsOpt = customLevel->CustomSaveDataInfo->get().TryGetBasicLevelDetails();
                if (!levelDetailsOpt.has_value()) break;

                // DEBUG("Setting requirements from custom level, level details ptr: {}", fmt::ptr(&levelDetailsOpt.value()));
                auto& levelDetails = levelDetailsOpt->get();
                auto difficultyDetailsOpt = levelDetails.TryGetCharacteristicAndDifficulty(beatmapKey.beatmapCharacteristic->serializedName, beatmapKey.difficulty);

                if (difficultyDetailsOpt.has_value()) {
                    auto& difficultyDetails = difficultyDetailsOpt->get();

                    DEBUG("Requirements available, difficultyDetails.requirements.size(): {}", difficultyDetails.requirements.size());
                    for (auto& req : difficultyDetails.requirements) {
                        static ConstString RequirementFound("Requirement Found");
                        static ConstString RequirementMissing("Requirement Missing");

                        auto cell = GetCellInfo();
                        bool installed = _capabilities->IsCapabilityRegistered(req);
                        cell->text = fmt::format("<size=75%>{}", req);
                        cell->subText = installed ? RequirementFound : RequirementMissing;
                        cell->icon = installed ? get_HaveReqIcon() : get_MissingReqIcon();
                        _levelInfoCells.push_back(cell);
                    }
                } else DEBUG("Setting requirements from custom level, difficultyDetailsOpt has no value");

                DEBUG("Setting requirements from custom level, levelDetails.contributors.size(): {}", levelDetails.contributors.size());
                for (auto& contributor : levelDetails.contributors) {
                    auto cell = GetCellInfo();
                    DEBUG("Setting contributors ptr: {}, name ptr: {}, role ptr: {}", fmt::ptr(&contributor), fmt::ptr(&contributor.name), fmt::ptr(&contributor.role));
                    cell->text = fmt::format("<size=75%>{}", contributor.name);
                    cell->subText = contributor.role;
                    cell->icon = get_InfoIcon();
                    _levelInfoCells.push_back(cell);
                }

                DEBUG("Setting requirements from custom level, difficultyDetails.customColors has value: {}", difficultyDetailsOpt.has_value());
                if (difficultyDetailsOpt.has_value()) {
                    auto& difficultyDetails = difficultyDetailsOpt->get();
                    if (difficultyDetails.customColors.has_value()) {
                        static ConstString CustomColorsText("<size=75%>Custom Colors Available");
                        static ConstString CustomColorsSubText("Click here to preview & enable or disable it.");

                        auto cell = GetCellInfo();
                        cell->text = CustomColorsText;
                        cell->subText = CustomColorsSubText;
                        cell->icon = get_ColorsIcon();
                        _levelInfoCells.push_back(cell);

                        DEBUG("Calling AcceptColors with custom colors");
                        _colorsUI->AcceptColors(difficultyDetails.customColors.value());
                    }
                }
            } break;
            // nullptr
            case false: break;
        }

        UpdateDataCells();
    }

    void MpRequirementsUI::SetRequirementsFromPacket(Beatmaps::Packets::MpBeatmapPacket* packet) {
        ClearCells(_levelInfoCells);
        auto diff = (uint8_t)packet->difficulty.value__;
        auto diffReqsItr = packet->requirements.find(diff);

        if (diffReqsItr != packet->requirements.end()) {
            for (auto& req : diffReqsItr->second) {
                static ConstString RequirementFound("Requirement Found");
                static ConstString RequirementMissing("Requirement Missing");

                auto cell = GetCellInfo();
                bool installed = _capabilities->IsCapabilityRegistered(req);
                cell->text = fmt::format("<size=75%>{}", req);
                cell->subText = installed ? RequirementFound : RequirementMissing;
                cell->icon = installed ? get_HaveReqIcon() : get_MissingReqIcon();
                _levelInfoCells.push_back(cell);
            }
        }

        for (auto& contributor : packet->contributors) {
            auto cell = GetCellInfo();
            cell->text = fmt::format("<size=75%>{}", contributor.name);
            cell->subText = contributor.role;
            cell->icon = get_InfoIcon();
            _levelInfoCells.push_back(cell);
        }

        auto colors = packet->mapColors.find(diff);
        if (colors != packet->mapColors.end()) {
            static ConstString CustomColorsText("<size=75%>Custom Colors Available");
            static ConstString CustomColorsSubText("Click here to preview & enable or disable it.");

            auto cell = GetCellInfo();
            cell->text = CustomColorsText;
            cell->subText = CustomColorsSubText;
            cell->icon = get_ColorsIcon();
            _levelInfoCells.push_back(cell);
        }

        UpdateDataCells();
    }

    void MpRequirementsUI::SetNoRequirementsFound() {
        ClearCells(_levelInfoCells);
        UpdateDataCells();
    }

    void MpRequirementsUI::PlayButtonDisablingModsChanged(std::span<SongCore::API::PlayButton::PlayButtonDisablingModInfo const> disablingModsInfos) {
        SetDisablingModInfoCells(disablingModsInfos);

        UpdateDataCells();
    }

    void MpRequirementsUI::SetDisablingModInfoCells(std::span<SongCore::API::PlayButton::PlayButtonDisablingModInfo const> disablingModsInfos) {
        ClearCells(_disablingModsCells);

        if (disablingModsInfos.empty()) return;
        auto cell = GetCellInfo();
        cell->text = fmt::format("Play button disabled");
        cell->icon = get_WarningIcon();
        cell->subText = "The following mods are responsible:";
        _disablingModsCells.push_back(cell);

        for (auto& modInfo : disablingModsInfos) {
            static auto NoReason = ConstString("No reason given");
            auto cell = GetCellInfo();
            cell->text = modInfo.modID;
            cell->icon = get_WarningIcon();
            cell->subText = modInfo.reason.empty() ? NoReason : StringW(modInfo.reason);
            _disablingModsCells.push_back(cell);
        }
    }

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
        if (!_##icon##Icon || _##icon##Icon->m_CachedPtr.IsNull()) { \
            auto& data = Assets::icon##_png;\
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
