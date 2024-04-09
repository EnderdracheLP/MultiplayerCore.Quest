#include "hooking.hpp"
#include "logging.hpp"

#include "GlobalNamespace/GameServerPlayerTableCell.hpp"
#include "GlobalNamespace/GameServerPlayersTableView.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UI/GameServerPlayerTableCellCustomData.hpp"

MAKE_AUTO_HOOK_MATCH(
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
    // we could be getting an exception because GetBeatmapLevel might return null in this method if the selected level is not installed here
    // maybe we should just reimplement this method here...
    // maybe we even have to wrap this in a runmethod invoke to do a cursed catch of the exception...
    try {
        GameServerPlayerTableCell_SetData(self, connectedPlayer, playerData, hasKickPermissions, allowSelection, getLevelEntitlementTask);
    } catch (std::exception const& e) {
        // caught an exception!
    } catch (...) {
        // caught unknown exception
    }

    // get our custom data component and update it
    auto customData = self->GetComponent<MultiplayerCore::UI::GameServerPlayerTableCellCustomData*>();
    if (customData) customData->UpdateDisplayedInfo(connectedPlayer, playerData);
}

// add our component to the prefab if neccesary. this way we get injected when the prefab is instantiated and also we ensure any of the prefabs has our component
MAKE_AUTO_HOOK_MATCH(GameServerPlayersTableView_GetCurrentPrefab, &GlobalNamespace::GameServerPlayersTableView::GetCurrentPrefab, UnityW<GlobalNamespace::GameServerPlayerTableCell>, GlobalNamespace::GameServerPlayersTableView* self) {
    auto res = GameServerPlayersTableView_GetCurrentPrefab(self);
    if (!res->GetComponent<MultiplayerCore::UI::GameServerPlayerTableCellCustomData*>()) res->gameObject->AddComponent<MultiplayerCore::UI::GameServerPlayerTableCellCustomData*>();
    return res;
}
