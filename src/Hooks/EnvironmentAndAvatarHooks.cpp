#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "GlobalFields.hpp"
#include "Hooks/SessionManagerAndExtendedPlayerHooks.hpp"

#include "GlobalNamespace/MultiplayerLobbyController.hpp"
#include "GlobalNamespace/LightWithIdMonoBehaviour.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarManager.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarPlace.hpp"
#include "GlobalNamespace/MultiplayerPlayerPlacement.hpp"
#include "GlobalNamespace/MultiplayerPlayerLayout.hpp"
#include "GlobalNamespace/ILobbyStateDataModel.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarPlaceManager.hpp"
#include "GlobalNamespace/MenuEnvironmentManager.hpp"
#include "GlobalNamespace/MultiplayerLobbyCenterStageManager.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarManager.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "GlobalNamespace/ConnectedPlayerManager_ConnectedPlayer.hpp"
#include "GlobalNamespace/AvatarPoseRestrictions.hpp"
#include "GlobalNamespace/GameplayServerConfiguration.hpp"

#include "System/Collections/Generic/List_1.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"

//Nothing here causes blackscreen crash

using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections::Generic;

namespace MultiplayerCore {

#pragma region Fields
    float innerCircleRadius;
    float minOuterCircleRadius;
    float angleBetweenPlayersWithEvenAdjustment;
    float outerCircleRadius;

    bool initialized;

    void HandleLobbyEnvironmentLoaded(ILobbyStateDataModel* _lobbyStateDataModel, MenuEnvironmentManager* _menuEnvironmentManager, MultiplayerLobbyAvatarPlaceManager* _placeManager, MultiplayerLobbyCenterStageManager* _stageManager) {
        initialized = false;
        getLogger().debug("HandleLobbyEnvironmentLoaded Started");

        innerCircleRadius = _placeManager->innerCircleRadius;
        minOuterCircleRadius = _placeManager->minOuterCircleRadius;
        //getLogger().debug("innerCircleRadius %f, minOuterCircleRadius %f", innerCircleRadius, minOuterCircleRadius);
        angleBetweenPlayersWithEvenAdjustment = MultiplayerPlayerPlacement::GetAngleBetweenPlayersWithEvenAdjustment(_lobbyStateDataModel->get_configuration().maxPlayerCount, MultiplayerPlayerLayout::Circle);
        outerCircleRadius = fmax(MultiplayerPlayerPlacement::GetOuterCircleRadius(angleBetweenPlayersWithEvenAdjustment, innerCircleRadius), minOuterCircleRadius);
        //getLogger().debug("angleBetweenPlayersWithEvenAdjustment %f, outerCircleRadius %f", angleBetweenPlayersWithEvenAdjustment, outerCircleRadius);

        bool buildingsEnabled = (_multiplayerSessionManager->maxPlayerCount <= 18);
        auto* Construction_tr = _menuEnvironmentManager->get_transform()->Find("Construction");
        if (Construction_tr && Construction_tr->get_gameObject()) {
            Construction_tr->get_gameObject()->SetActive(buildingsEnabled);
        }

        auto* Construction_1_tr = _menuEnvironmentManager->get_transform()->Find("Construction (1)");
        if (Construction_1_tr && Construction_1_tr->get_gameObject()) {
            Construction_1_tr->get_gameObject()->SetActive(buildingsEnabled);
        }

        float centerScreenScale = outerCircleRadius / minOuterCircleRadius;
        _stageManager->get_transform()->set_localScale({ centerScreenScale, centerScreenScale, centerScreenScale });

        initialized = true;
        getLogger().debug("HandleLobbyEnvironmentLoaded Finished");
    }

    MAKE_HOOK_MATCH(MultiplayerLobbyController_ActivateMultiplayerLobby, &MultiplayerLobbyController::ActivateMultiplayerLobby, void, MultiplayerLobbyController* self) {
        getLogger().debug("ActivateMultiplayerLobby Start");
        MultiplayerLobbyAvatarPlaceManager* _placeManager = Resources::FindObjectsOfTypeAll<MultiplayerLobbyAvatarPlaceManager*>()[0];
        MenuEnvironmentManager* _menuEnvironmentManager = Resources::FindObjectsOfTypeAll<MenuEnvironmentManager*>()[0];
        MultiplayerLobbyCenterStageManager* _stageManager = Resources::FindObjectsOfTypeAll<MultiplayerLobbyCenterStageManager*>()[0];
        ILobbyStateDataModel* _lobbyStateDataModel = _placeManager->lobbyStateDataModel;

        self->innerCircleRadius = 1;
        self->minOuterCircleRadius = 4.4f;
        MultiplayerLobbyController_ActivateMultiplayerLobby(self);

        HandleLobbyEnvironmentLoaded(_lobbyStateDataModel, _menuEnvironmentManager, _placeManager, _stageManager);
        getLogger().debug("ActivateMultiplayerLobby Done");
    }

#pragma endregion

    MAKE_HOOK_MATCH(AvatarPoseRestrictions_HandleAvatarPoseControllerPositionsWillBeSet, &AvatarPoseRestrictions::HandleAvatarPoseControllerPositionsWillBeSet, void, AvatarPoseRestrictions* self, Quaternion headRotation, Vector3 headPosition, Vector3 leftHandPosition, Vector3 rightHandPosition, ByRef<Vector3> newHeadPosition, ByRef<Vector3> newLeftHandPosition, ByRef<Vector3> newRightHandPosition) {
        newHeadPosition.heldRef = headPosition;
        newLeftHandPosition.heldRef = self->LimitHandPositionRelativeToHead(leftHandPosition, headPosition);
        newRightHandPosition.heldRef = self->LimitHandPositionRelativeToHead(rightHandPosition, headPosition);
    }

    void Hooks::EnvironmentHooks() {
        INSTALL_HOOK(getLogger(), MultiplayerLobbyController_ActivateMultiplayerLobby);
        INSTALL_HOOK_ORIG(getLogger(), AvatarPoseRestrictions_HandleAvatarPoseControllerPositionsWillBeSet);
    }
}