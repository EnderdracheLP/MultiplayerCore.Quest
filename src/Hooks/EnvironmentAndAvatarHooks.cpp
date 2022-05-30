#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "GlobalFields.hpp"
#include "shared/GlobalFields.hpp"
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
#include "GlobalNamespace/MultiplayerLayoutProvider.hpp"

#include "System/Collections/Generic/List_1.hpp"
#include "System/Action_2.hpp"

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

    static bool addEmptyPlayerSlotForEvenCount = false;

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
        MultiplayerLobbyAvatarPlaceManager* _placeManager = self->multiplayerLobbyAvatarPlaceManager;
        MenuEnvironmentManager* _menuEnvironmentManager = self->menuEnvironmentManager;
        MultiplayerLobbyCenterStageManager* _stageManager = self->multiplayerLobbyCenterStageManager;
        ILobbyStateDataModel* _lobbyStateDataModel = _placeManager->lobbyStateDataModel;

        self->innerCircleRadius = 1;
        self->minOuterCircleRadius = 4.4f;
        MultiplayerLobbyController_ActivateMultiplayerLobby(self);

        HandleLobbyEnvironmentLoaded(_lobbyStateDataModel, _menuEnvironmentManager, _placeManager, _stageManager);
        getLogger().debug("ActivateMultiplayerLobby Done");
    }

    MAKE_HOOK_MATCH(MultiplayerPlayerPlacement_GetAngleBetweenPlayersWithEvenAdjustment, &MultiplayerPlayerPlacement::GetAngleBetweenPlayersWithEvenAdjustment, float, int numberOfPlayers, MultiplayerPlayerLayout layout) {
        if (addEmptyPlayerSlotForEvenCount && numberOfPlayers % 2 == 0 && layout != MultiplayerPlayerLayout::Duel)
		{
			numberOfPlayers++;
		}
        return 360.0f / (float)numberOfPlayers;
    }

    MAKE_HOOK_MATCH(MultiplayerLayoutProvider_CalculateLayout, &MultiplayerLayoutProvider::CalculateLayout, GlobalNamespace::MultiplayerPlayerLayout, MultiplayerLayoutProvider* self, int activePlayersCount) {
        if (activePlayersCount == 2)
		{
			self->layout = MultiplayerPlayerLayout::Duel;
		}
		else
		{
			self->layout = MultiplayerPlayerLayout::Circle;
		}
		if (addEmptyPlayerSlotForEvenCount && activePlayersCount % 2 == 0 && self->layout != MultiplayerPlayerLayout::Duel)
		{
			activePlayersCount++;
		}
		self->activePlayerSpotsCount = activePlayersCount;
		::System::Action_2<::GlobalNamespace::MultiplayerPlayerLayout, int>* action = self->playersLayoutWasCalculatedEvent;
		if (action != nullptr)
		{
			action->Invoke(self->layout, activePlayersCount);
		}
		return self->layout;
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
        INSTALL_HOOK_ORIG(getLogger(), MultiplayerPlayerPlacement_GetAngleBetweenPlayersWithEvenAdjustment);
        INSTALL_HOOK_ORIG(getLogger(), MultiplayerLayoutProvider_CalculateLayout);
    }
}