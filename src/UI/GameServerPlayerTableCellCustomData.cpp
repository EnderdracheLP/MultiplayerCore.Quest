#include "UI/GameServerPlayerTableCellCustomData.hpp"
#include "Utilities.hpp"
#include "bsml/shared/Helpers/delegates.hpp"

#include "TMPro/TextMeshProUGUI.hpp"
#include "HMUI/UIItemsList_1.hpp"
#include "HMUI/HoverHint.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/BeatmapLevelsModel.hpp"
#include "GlobalNamespace/ILevelGameplaySetupData.hpp"
#include "GlobalNamespace/GameplayModifiersModelSO.hpp"
#include "GlobalNamespace/BeatmapDifficultyMethods.hpp"
#include "GlobalNamespace/GameplayModifierInfoListItem.hpp"
#include "UnityEngine/UI/Image.hpp"
#include "UnityEngine/UI/Selectable.hpp"
#include "UnityEngine/GameObject.hpp"
#include "BGLib/Polyglot/Localization.hpp"

DEFINE_TYPE(MultiplayerCore::UI, GameServerPlayerTableCellCustomData);

namespace MultiplayerCore::UI {
    void GameServerPlayerTableCellCustomData::Inject(MultiplayerCore::Objects::MpPlayersDataModel* mpPlayersDataModel) {
        _mpPlayersDataModel = mpPlayersDataModel;
    }

    void GameServerPlayerTableCellCustomData::Awake() {
        _gameServerPlayerTableCell = GetComponent<GlobalNamespace::GameServerPlayerTableCell*>();
    }

    void GameServerPlayerTableCellCustomData::SetData(GlobalNamespace::IConnectedPlayer* connectedPlayer, GlobalNamespace::ILobbyPlayerData* playerData, bool hasKickPermissions, bool allowSelection, System::Threading::Tasks::Task_1<GlobalNamespace::EntitlementStatus>* getLevelEntitlementTask) {
        if (!_gameServerPlayerTableCell) return;
        _gameServerPlayerTableCell->_playerNameText->text = connectedPlayer->userName;
        if (!playerData->isReady && playerData->isActive && !playerData->isPartyOwner) {
            _gameServerPlayerTableCell->_statusImageView->enabled = false;
        } else {
            auto statusView = _gameServerPlayerTableCell->_statusImageView;

            statusView->enabled = true;
            if (playerData->isPartyOwner) statusView->sprite = _gameServerPlayerTableCell->_hostIcon;
            else if (playerData->isActive) statusView->sprite = _gameServerPlayerTableCell->_readyIcon;
            else statusView->sprite = _gameServerPlayerTableCell->_spectatingIcon;
        }

        auto key = playerData->i___GlobalNamespace__ILevelGameplaySetupData()->beatmapKey;
        bool validKey = key.IsValid();
        bool displayLevelText = validKey;
        if (validKey) {
            auto level = _gameServerPlayerTableCell->_beatmapLevelsModel->GetBeatmapLevel(key.levelId);
            auto levelHash = HashFromLevelID(key.levelId);
            _gameServerPlayerTableCell->_suggestedLevelText->text = level ? level->songName : nullptr;
            displayLevelText = level != nullptr;
            if (!level && _mpPlayersDataModel && !levelHash.empty()) {
                auto packet = _mpPlayersDataModel->FindLevelPacket(levelHash);
                _gameServerPlayerTableCell->_suggestedLevelText->text = packet ? packet->songName : nullptr;
                displayLevelText = packet != nullptr;
            }

            _gameServerPlayerTableCell->_suggestedCharacteristicIcon->sprite = key.beatmapCharacteristic->icon;
            _gameServerPlayerTableCell->_suggestedDifficultyText->text = GlobalNamespace::BeatmapDifficultyMethods::ShortName(key.difficulty);
        }
        SetLevelFoundValues(displayLevelText);

        auto modifiers = playerData->i___GlobalNamespace__ILevelGameplaySetupData()->gameplayModifiers;
        bool anyModifiers = modifiers && !modifiers->IsWithoutModifiers();
        _gameServerPlayerTableCell->_suggestedModifiersList->gameObject->SetActive(anyModifiers);
        _gameServerPlayerTableCell->_emptySuggestedModifiersText->gameObject->SetActive(!anyModifiers);

        if (anyModifiers) {
            auto modifiersList = _gameServerPlayerTableCell->_gameplayModifiers->CreateModifierParamsList(modifiers);
            auto count = modifiersList->Count;
            _gameServerPlayerTableCell->_emptySuggestedModifiersText->gameObject->SetActive(count == 0);
            if (count > 0) {
                auto cb = BSML::MakeDelegate<HMUI::UIItemsList_1<UnityW<GlobalNamespace::GameplayModifierInfoListItem>>::DataCallback*>(
                    [&modifiersList](int id, UnityW<GlobalNamespace::GameplayModifierInfoListItem> item){
                        item->SetModifier(modifiersList->get_Item(id), false);
                    }
                );
                _gameServerPlayerTableCell->_suggestedModifiersList->SetData(count, cb);
            }
        }

        _gameServerPlayerTableCell->_useModifiersButton->interactable = !connectedPlayer->isMe && anyModifiers && allowSelection;
        _gameServerPlayerTableCell->_kickPlayerButton->interactable = !connectedPlayer->isMe && hasKickPermissions && allowSelection;
        _gameServerPlayerTableCell->_mutePlayerButton->gameObject->SetActive(false);
        if (getLevelEntitlementTask && !connectedPlayer->isMe) {
            static ConstString label("LABEL_CANT_START_GAME_DO_NOT_OWN_SONG");
            _gameServerPlayerTableCell->_useBeatmapButtonHoverHint->text = BGLib::Polyglot::Localization::Get(label);
            _gameServerPlayerTableCell->SetBeatmapUseButtonEnabledAsync(getLevelEntitlementTask);
            return;
        }

        _gameServerPlayerTableCell->_useBeatmapButton->interactable = false;
        _gameServerPlayerTableCell->_useBeatmapButtonHoverHint->enabled = false;
    }

    void GameServerPlayerTableCellCustomData::SetLevelFoundValues(bool displayLevelText) {
        if (!_gameServerPlayerTableCell) return;
        _gameServerPlayerTableCell->_suggestedLevelText->gameObject->SetActive(displayLevelText);
        _gameServerPlayerTableCell->_suggestedCharacteristicIcon->gameObject->SetActive(displayLevelText);
        _gameServerPlayerTableCell->_suggestedDifficultyText->gameObject->SetActive(displayLevelText);
        _gameServerPlayerTableCell->_emptySuggestedLevelText->gameObject->SetActive(!displayLevelText);
    }
}
