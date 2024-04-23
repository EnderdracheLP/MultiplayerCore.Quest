#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "../Objects/MpPlayersDataModel.hpp"
#include "GlobalNamespace/GameServerPlayerTableCell.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "GlobalNamespace/ILobbyPlayerData.hpp"

DECLARE_CLASS_CODEGEN(MultiplayerCore::UI, GameServerPlayerTableCellCustomData, UnityEngine::MonoBehaviour,
        DECLARE_INJECT_METHOD(void, Inject, MultiplayerCore::Objects::MpPlayersDataModel* mpPlayersDataModel);
        DECLARE_INSTANCE_FIELD_PRIVATE(MultiplayerCore::Objects::MpPlayersDataModel*, _mpPlayersDataModel);
        DECLARE_INSTANCE_FIELD(GlobalNamespace::GameServerPlayerTableCell*, _gameServerPlayerTableCell);

        DECLARE_INSTANCE_METHOD(void, Awake);
    public:
        void SetData(GlobalNamespace::IConnectedPlayer* connectedPlayer, GlobalNamespace::ILobbyPlayerData* playerData, bool hasKickPermissions, bool allowSelection, System::Threading::Tasks::Task_1<GlobalNamespace::EntitlementStatus>* getLevelEntitlementTask);
    private:
        void SetLevelFoundValues(bool displayLevelText);
)
