#include "UI/GameServerPlayerTableCellCustomData.hpp"
#include "GlobalNamespace/ILevelGameplaySetupData.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "Utilities.hpp"

DEFINE_TYPE(MultiplayerCore::UI, GameServerPlayerTableCellCustomData);

namespace MultiplayerCore::UI {
    void GameServerPlayerTableCellCustomData::Inject(MultiplayerCore::Objects::MpPlayersDataModel* mpPlayersDataModel) {
        _mpPlayersDataModel = mpPlayersDataModel;
    }

    void GameServerPlayerTableCellCustomData::Awake() {
        _gameServerPlayerTableCell = GetComponent<GlobalNamespace::GameServerPlayerTableCell*>();
    }

    void GameServerPlayerTableCellCustomData::UpdateDisplayedInfo(GlobalNamespace::IConnectedPlayer* connectedPlayer, GlobalNamespace::ILobbyPlayerData* playerData) {
        // if not set just return to be safe
        if (!_gameServerPlayerTableCell) return;

        auto beatmapKey = playerData->i___GlobalNamespace__ILevelGameplaySetupData()->beatmapKey;
        if (!beatmapKey.IsValid()) return;

        auto hash = HashFromLevelID(beatmapKey.levelId);
        if (hash.empty()) return;

        // get the packet for this player
        auto packet = _mpPlayersDataModel->PlayerPacket[connectedPlayer->userId];

        // no packet or packet hash mismatch, just return
        if (!packet || packet->levelHash != hash) return;

        // update the level name text from the gotten packet
        _gameServerPlayerTableCell->_suggestedLevelText->text = packet->songName;
    }
}
