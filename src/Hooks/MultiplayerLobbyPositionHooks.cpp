#include "hooking.hpp"
#include "logging.hpp"

#include "GlobalNamespace/MultiplayerLobbyAvatarManager.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarPlaceManager.hpp"
#include "GlobalNamespace/MultiplayerLobbyCenterStageManager.hpp"
#include "GlobalNamespace/MultiplayerPlayersManager.hpp"
#include "GlobalNamespace/MultiplayerPlayerStartState.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarController.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarController_Factory.hpp"
#include "GlobalNamespace/MultiplayerPlayerPlacement.hpp"
#include "GlobalNamespace/MultiplayerPlayerLayout.hpp"
#include "GlobalNamespace/MultiplayerLayoutProvider.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnCenter.hpp"
#include "GlobalNamespace/GameplayServerConfiguration.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "GlobalNamespace/ILobbyStateDataModel.hpp"
#include "GlobalNamespace/IMultiplayerSessionManager.hpp"
#include "GlobalNamespace/ConnectedPlayerHelpers.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Transform.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;
// reimplementation so methods get called like shown
MAKE_AUTO_HOOK_ORIG_MATCH(MultiplayerLobbyAvatarManager_AddPlayer, &MultiplayerLobbyAvatarManager::AddPlayer, void, MultiplayerLobbyAvatarManager* self, IConnectedPlayer* connectedPlayer) {
    if (connectedPlayer->get_isMe()) return;
    auto userId = connectedPlayer->get_userId();
	MultiplayerLobbyAvatarController* multiplayerLobbyAvatarController;
    if (self->playerIdToAvatarMap->TryGetValue(userId, byref(multiplayerLobbyAvatarController))) {
        multiplayerLobbyAvatarController->DestroySelf();
        self->playerIdToAvatarMap->Remove(userId);
    }

    multiplayerLobbyAvatarController = self->avatarControllerFactory->Create(connectedPlayer);
    self->playerIdToAvatarMap->Add(userId, multiplayerLobbyAvatarController);

    float angleBetweenPlayersWithEvenAdjustment = MultiplayerPlayerPlacement::GetAngleBetweenPlayersWithEvenAdjustment(self->lobbyStateDataModel->get_configuration().maxPlayerCount, MultiplayerPlayerLayout::Circle);
    float outerCircleRadius = std::max(MultiplayerPlayerPlacement::GetOuterCircleRadius(angleBetweenPlayersWithEvenAdjustment, self->innerCircleRadius), self->minOuterCircleRadius);
    float outerCirclePositionAngleForPlayer = MultiplayerPlayerPlacement::GetOuterCirclePositionAngleForPlayer(connectedPlayer->get_sortIndex(), self->lobbyStateDataModel->get_localPlayer()->get_sortIndex(), angleBetweenPlayersWithEvenAdjustment);
    auto playerWorldPosition = MultiplayerPlayerPlacement::GetPlayerWorldPosition(outerCircleRadius, outerCirclePositionAngleForPlayer, MultiplayerPlayerLayout::Circle);
    multiplayerLobbyAvatarController->ShowSpawnAnimation(playerWorldPosition, Quaternion::Euler(0, outerCirclePositionAngleForPlayer, 0));
}

MAKE_AUTO_HOOK_ORIG_MATCH(MultiplayerLobbyAvatarPlaceManager_SpawnAllPlaces, &MultiplayerLobbyAvatarPlaceManager::SpawnAllPlaces, void, MultiplayerLobbyAvatarPlaceManager* self) {
    MultiplayerLobbyAvatarPlaceManager_SpawnAllPlaces(self);

    // fix positions and rotations of the places
    float angleBetweenPlayersWithEvenAdjustment = MultiplayerPlayerPlacement::GetAngleBetweenPlayersWithEvenAdjustment(self->lobbyStateDataModel->get_configuration().maxPlayerCount, MultiplayerPlayerLayout::Circle);
    float outerCircleRadius = std::max(MultiplayerPlayerPlacement::GetOuterCircleRadius(angleBetweenPlayersWithEvenAdjustment, self->innerCircleRadius), self->minOuterCircleRadius);
    int localPlayerSortIndex = self->lobbyStateDataModel->get_localPlayer()->get_sortIndex();
    for (int idx = -1; auto multiplayerLobbyAvatarPlace : ListW<MultiplayerLobbyAvatarPlace*>(self->allPlaces)) {
        idx++;

        if (idx != localPlayerSortIndex) {
            auto outerCirclePositionAngleForPlayer = MultiplayerPlayerPlacement::GetOuterCirclePositionAngleForPlayer(idx, localPlayerSortIndex, angleBetweenPlayersWithEvenAdjustment);
            auto playerWorldPosition = MultiplayerPlayerPlacement::GetPlayerWorldPosition(outerCircleRadius, outerCirclePositionAngleForPlayer, MultiplayerPlayerLayout::Circle);
            multiplayerLobbyAvatarPlace->SetPositionAndRotation(playerWorldPosition, Quaternion::Euler(0, outerCirclePositionAngleForPlayer, 0));
        }
    }
}

// reimplementation so methods get called like shown
MAKE_AUTO_HOOK_ORIG_MATCH(MultiplayerLobbyCenterStageManager_RecalculateCenterPosition, &MultiplayerLobbyCenterStageManager::RecalculateCenterPosition, void, MultiplayerLobbyCenterStageManager* self) {
	float angleBetweenPlayersWithEvenAdjustment = MultiplayerPlayerPlacement::GetAngleBetweenPlayersWithEvenAdjustment(self->lobbyStateDataModel->get_configuration().maxPlayerCount, MultiplayerPlayerLayout::Circle);
	self->centerObjectTransform->set_localPosition({0, 0, std::max(MultiplayerPlayerPlacement::GetOuterCircleRadius(angleBetweenPlayersWithEvenAdjustment, self->innerCircleRadius), self->minOuterCircleRadius)});
}

MAKE_AUTO_HOOK_ORIG_MATCH(MultiplayerPlayersManager_SpawnPlayers, &MultiplayerPlayersManager::SpawnPlayers, void, MultiplayerPlayersManager* self, MultiplayerPlayerStartState localPlayerStartState, System::Collections::Generic::IReadOnlyList_1<IConnectedPlayer*>* allActiveAtGameStartPlayers) {
    MultiplayerPlayersManager_SpawnPlayers(self, localPlayerStartState, allActiveAtGameStartPlayers);
    ListW<IConnectedPlayer*> allActiveAtGameStartPlayersW(self->allActiveAtGameStartPlayers);

    // if found, it's the index, if not found, it is end - begin == count
    auto localPlayer = self->multiplayerSessionManager->get_localPlayer();
    int localPlayerIndex = std::find(allActiveAtGameStartPlayersW.begin(), allActiveAtGameStartPlayersW.end(), localPlayer) - allActiveAtGameStartPlayersW.begin();

    auto outerCircleRadius = self->beatmapObjectSpawnCenter->CalculateSpawnCenterPosition(allActiveAtGameStartPlayersW.size());
    float angleBetweenPlayersWithEvenAdjustment = MultiplayerPlayerPlacement::GetAngleBetweenPlayersWithEvenAdjustment(self->allActiveAtGameStartPlayers->i_IReadOnlyCollection_1_T()->get_Count(), MultiplayerPlayerLayout::Circle);
    auto layout = self->layoutProvider->CalculateLayout(allActiveAtGameStartPlayersW.size());
    for (int idx = -1; auto connectedPlayer : allActiveAtGameStartPlayersW) {
        idx++;
        if (connectedPlayer->get_isMe()) continue;
        auto userId = connectedPlayer->get_userId();
        auto outerCirclePositionAngleForPlayer = MultiplayerPlayerPlacement::GetOuterCirclePositionAngleForPlayer(idx, localPlayerIndex, angleBetweenPlayersWithEvenAdjustment);
        self->connectedPlayerCenterFacingRotationsMap->set_Item(userId, outerCirclePositionAngleForPlayer);

        if (!ConnectedPlayerHelpers::IsFailed(connectedPlayer) && ConnectedPlayerHelpers::WasActiveAtLevelStart(connectedPlayer)) {
            auto playerWorldPosition = MultiplayerPlayerPlacement::GetPlayerWorldPosition(outerCircleRadius, outerCirclePositionAngleForPlayer, layout);
            auto multiplayerConnectedFacade = self->connectedPlayerControllersMap->get_Item(userId);

            auto t = multiplayerConnectedFacade->get_transform();
            t->set_position(playerWorldPosition);
            t->set_rotation(Quaternion::Euler(0, outerCirclePositionAngleForPlayer, 0));
        }
    }
}
