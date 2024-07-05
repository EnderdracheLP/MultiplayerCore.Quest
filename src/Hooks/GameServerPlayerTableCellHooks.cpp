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
        DEBUG("Custom data found, calling custom");
        return customData->SetData(connectedPlayer, playerData, hasKickPermissions, allowSelection, getLevelEntitlementTask);
    } else {
        // Test, adding component if not found
        DEBUG("No custom data found, adding component and initializing it");
        customData = self->gameObject->AddComponent<MultiplayerCore::UI::GameServerPlayerTableCellCustomData*>();
        if (!customData) {
            ERROR("Failed to add custom data component. This should never happen, calling orig and praying");
            return GameServerPlayerTableCell_SetData(self, connectedPlayer, playerData, hasKickPermissions, allowSelection, getLevelEntitlementTask);
        }
        customData->Awake();
        return customData->SetData(connectedPlayer, playerData, hasKickPermissions, allowSelection, getLevelEntitlementTask);
    }
}

// add our component to the prefab if neccesary. this way we get injected when the prefab is instantiated and also we ensure any of the prefabs has our component
// WARNING, this only applies to cells of other players not the local player
MAKE_AUTO_HOOK_MATCH(GameServerPlayersTableView_GetCurrentPrefab, &GlobalNamespace::GameServerPlayersTableView::GetCurrentPrefab, UnityW<GlobalNamespace::GameServerPlayerTableCell>, GlobalNamespace::GameServerPlayersTableView* self) {
    auto res = GameServerPlayersTableView_GetCurrentPrefab(self);
    if (!res->GetComponent<MultiplayerCore::UI::GameServerPlayerTableCellCustomData*>()) res->gameObject->AddComponent<MultiplayerCore::UI::GameServerPlayerTableCellCustomData*>();
    return res;
}