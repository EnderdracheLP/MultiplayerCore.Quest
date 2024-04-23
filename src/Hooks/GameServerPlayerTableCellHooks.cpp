#include "hooking.hpp"
#include "logging.hpp"

#include "GlobalNamespace/GameServerPlayerTableCell.hpp"
#include "GlobalNamespace/GameServerPlayersTableView.hpp"
#include "GlobalNamespace/ILevelGameplaySetupData.hpp"
#include "GlobalNamespace/CenterStageScreenController.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UI/GameServerPlayerTableCellCustomData.hpp"
#include "Utilities.hpp"

// does not call orig
MAKE_AUTO_HOOK_ORIG_MATCH(
    GameServerPlayerTableCell_SetData,
    &GlobalNamespace::GameServerPlayerTableCell::SetData,
    void,
    GlobalNamespace::GameServerPlayerTableCell* self,
    GlobalNamespace::IConnectedPlayer* connectedPlayer,
    GlobalNamespace::ILobbyPlayerData* playerData,
    bool hasKickPermissions,
    bool allowSelection,
    System::Threading::Tasks::Task_1<GlobalNamespace::EntitlementStatus>* getLevelEntitlementTask
) {
    auto customData = self->GetComponent<MultiplayerCore::UI::GameServerPlayerTableCellCustomData*>();
    if (customData) {
        return customData->SetData(connectedPlayer, playerData, hasKickPermissions, allowSelection, getLevelEntitlementTask);
    } else {
        return GameServerPlayerTableCell_SetData(self, connectedPlayer, playerData, hasKickPermissions, allowSelection, getLevelEntitlementTask);
    }
}

// add our component to the prefab if neccesary. this way we get injected when the prefab is instantiated and also we ensure any of the prefabs has our component
MAKE_AUTO_HOOK_MATCH(GameServerPlayersTableView_GetCurrentPrefab, &GlobalNamespace::GameServerPlayersTableView::GetCurrentPrefab, UnityW<GlobalNamespace::GameServerPlayerTableCell>, GlobalNamespace::GameServerPlayersTableView* self) {
    auto res = GameServerPlayersTableView_GetCurrentPrefab(self);
    if (!res->GetComponent<MultiplayerCore::UI::GameServerPlayerTableCellCustomData*>()) res->gameObject->AddComponent<MultiplayerCore::UI::GameServerPlayerTableCellCustomData*>();
    return res;
}

MAKE_AUTO_HOOK_MATCH(
    CenterStageScreenController_SetNextGameplaySetupData,
    &GlobalNamespace::CenterStageScreenController::SetNextGameplaySetupData,
    void,
    GlobalNamespace::CenterStageScreenController* self,
    GlobalNamespace::ILevelGameplaySetupData* levelGameplaySetupData
) {
    auto key = levelGameplaySetupData->beatmapKey;
    if (key.IsValid()) {
        auto levelHash = MultiplayerCore::HashFromLevelID(key.levelId);
        if (!levelHash.empty()) {
        }
    }
    CenterStageScreenController_SetNextGameplaySetupData(self, levelGameplaySetupData);
}
