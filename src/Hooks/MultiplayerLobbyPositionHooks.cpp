#include "hooking.hpp"
#include "logging.hpp"

#include "GlobalNamespace/MultiplayerLobbyAvatarManager.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarPlaceManager.hpp"
#include "GlobalNamespace/MultiplayerLobbyCenterStageManager.hpp"
#include "GlobalNamespace/MultiplayerPlayersManager.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarPlace_Pool.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarPlace.hpp"
#include "GlobalNamespace/MultiplayerPlayerStartState.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarController.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarController_Factory.hpp"
#include "GlobalNamespace/MultiplayerLocalActivePlayerFacade_Factory.hpp"
#include "GlobalNamespace/MultiplayerLocalInactivePlayerFacade_Factory.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerFacade_Factory.hpp"
#include "GlobalNamespace/MultiplayerSpectatorController.hpp"
#include "GlobalNamespace/MultiplayerPlayerPlacement.hpp"
#include "GlobalNamespace/MultiplayerPlayerLayout.hpp"
#include "GlobalNamespace/MultiplayerLayoutProvider.hpp"
#include "GlobalNamespace/BeatmapObjectSpawnCenter.hpp"
#include "GlobalNamespace/GameplayServerConfiguration.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "GlobalNamespace/ILobbyStateDataModel.hpp"
#include "GlobalNamespace/IMultiplayerSessionManager.hpp"
#include "GlobalNamespace/ConnectedPlayerHelpers.hpp"
#include "GlobalNamespace/MultiplayerLevelCompletionResults.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Transform.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Action_1.hpp"

#include "custom-types/shared/delegate.hpp"
#include "bsml/shared/Helpers/delegates.hpp"

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

// reimplementation so methods get called like shown
MAKE_AUTO_HOOK_ORIG_MATCH(MultiplayerLobbyAvatarPlaceManager_SpawnAllPlaces, &MultiplayerLobbyAvatarPlaceManager::SpawnAllPlaces, void, MultiplayerLobbyAvatarPlaceManager* self) {
    self->DespawnAllPlaces();

    int maxPlayerCount = self->lobbyStateDataModel->get_configuration().maxPlayerCount;
    float angleBetweenPlayersWithEvenAdjustment = MultiplayerPlayerPlacement::GetAngleBetweenPlayersWithEvenAdjustment(maxPlayerCount, MultiplayerPlayerLayout::Circle);
    float outerCircleRadius = std::max(MultiplayerPlayerPlacement::GetOuterCircleRadius(angleBetweenPlayersWithEvenAdjustment, self->innerCircleRadius), self->minOuterCircleRadius);
    int localPlayerSortIndex = self->lobbyStateDataModel->get_localPlayer()->get_sortIndex();
    for (int i = 0; i < maxPlayerCount; i++) {
        if (i != localPlayerSortIndex) {
            auto outerCirclePositionAngleForPlayer = MultiplayerPlayerPlacement::GetOuterCirclePositionAngleForPlayer(i, localPlayerSortIndex, angleBetweenPlayersWithEvenAdjustment);
            auto playerWorldPosition = MultiplayerPlayerPlacement::GetPlayerWorldPosition(outerCircleRadius, outerCirclePositionAngleForPlayer, MultiplayerPlayerLayout::Circle);
            auto multiplayerLobbyAvatarPlace = self->avatarPlacesPool->Spawn();
            multiplayerLobbyAvatarPlace->SetPositionAndRotation(playerWorldPosition, Quaternion::Euler(0, outerCirclePositionAngleForPlayer, 0));
            self->allPlaces->Add(multiplayerLobbyAvatarPlace);
        }
    }
}

// reimplementation so methods get called like shown
MAKE_AUTO_HOOK_ORIG_MATCH(MultiplayerLobbyCenterStageManager_RecalculateCenterPosition, &MultiplayerLobbyCenterStageManager::RecalculateCenterPosition, void, MultiplayerLobbyCenterStageManager* self) {
	float angleBetweenPlayersWithEvenAdjustment = MultiplayerPlayerPlacement::GetAngleBetweenPlayersWithEvenAdjustment(self->lobbyStateDataModel->get_configuration().maxPlayerCount, MultiplayerPlayerLayout::Circle);
	self->centerObjectTransform->set_localPosition({0, 0, std::max(MultiplayerPlayerPlacement::GetOuterCircleRadius(angleBetweenPlayersWithEvenAdjustment, self->innerCircleRadius), self->minOuterCircleRadius)});
}

// reimplementation so methods get called like shown
MAKE_AUTO_HOOK_ORIG_MATCH(MultiplayerPlayersManager_SpawnPlayers, &MultiplayerPlayersManager::SpawnPlayers, void, MultiplayerPlayersManager* self, MultiplayerPlayerStartState localPlayerStartState, System::Collections::Generic::IReadOnlyList_1<IConnectedPlayer*>* allActiveAtGameStartPlayers) {
    self->connectedPlayerControllersMap->Clear();
    self->allActiveAtGameStartPlayers = allActiveAtGameStartPlayers;

    ListW<IConnectedPlayer*> allActiveAtGameStartPlayersW(self->allActiveAtGameStartPlayers);
    auto localPlayer = self->multiplayerSessionManager->get_localPlayer();
    auto layout = self->layoutProvider->CalculateLayout(allActiveAtGameStartPlayersW.size());
    auto outerCircleRadius = self->beatmapObjectSpawnCenter->CalculateSpawnCenterPosition(allActiveAtGameStartPlayersW.size());

    self->BindPlayerFactories(layout);

    if (ConnectedPlayerHelpers::WasActiveAtLevelStart(localPlayer) && localPlayerStartState == MultiplayerPlayerStartState::InSync) {
        self->activeLocalPlayerFacade = self->activeLocalPlayerFactory->Create(localPlayerStartState);
        self->currentEventsPublisher = self->activeLocalPlayerFacade->i_IMultiplayerLevelEndActionsPublisher();
        self->currentStartSeekSongControllerProvider = self->activeLocalPlayerFacade->i_IStartSeekSongControllerProvider();
    } else {
        self->inactiveLocalPlayerFacade = self->inactiveLocalPlayerFactory->Create(localPlayerStartState);
        self->currentEventsPublisher = self->inactiveLocalPlayerFacade->i_IMultiplayerLevelEndActionsPublisher();
        self->currentStartSeekSongControllerProvider = self->inactiveLocalPlayerFacade->i_IStartSeekSongControllerProvider();
        self->inactiveLocalPlayerFacade->spectatorController->SwitchToDefaultSpot();
        auto playerDidFinish = self->playerDidFinishEvent;
        if (playerDidFinish) playerDidFinish->Invoke(nullptr);
    }

    self->currentEventsPublisher->add_playerDidFinishEvent(
        BSML::MakeSystemAction<MultiplayerLevelCompletionResults*>(self, il2cpp_utils::il2cpp_type_check::MetadataGetter<&MultiplayerPlayersManager::HandlePlayerDidFinish>::get())
    );

    self->currentEventsPublisher->add_playerNetworkDidFailedEvent(
        BSML::MakeSystemAction<MultiplayerLevelCompletionResults*>(self, il2cpp_utils::il2cpp_type_check::MetadataGetter<&MultiplayerPlayersManager::HandlePlayerNetworkDidFailed>::get())
    );

    // if found, it's the index, if not found, it is end - begin == count
    int localPlayerIndex = std::find(allActiveAtGameStartPlayersW.begin(), allActiveAtGameStartPlayersW.end(), localPlayer) - allActiveAtGameStartPlayersW.begin();
    if (!ConnectedPlayerHelpers::WasActiveAtLevelStart(localPlayer)) localPlayerIndex = allActiveAtGameStartPlayersW.size();

    float angleBetweenPlayersWithEvenAdjustment = MultiplayerPlayerPlacement::GetAngleBetweenPlayersWithEvenAdjustment(allActiveAtGameStartPlayersW.size(), layout);
    for (int i = 0; i < allActiveAtGameStartPlayersW.size(); i++) {
        auto connectedPlayer = allActiveAtGameStartPlayersW[i];
        auto userId = connectedPlayer->get_userId();
        if (connectedPlayer->get_isMe()) {
            self->connectedPlayerCenterFacingRotationsMap->set_Item(userId, 0.0f);
            continue;
        }

        auto outerCirclePositionAngleForPlayer = MultiplayerPlayerPlacement::GetOuterCirclePositionAngleForPlayer(i, localPlayerIndex, angleBetweenPlayersWithEvenAdjustment);
        self->connectedPlayerCenterFacingRotationsMap->set_Item(userId, outerCirclePositionAngleForPlayer);

        if (!ConnectedPlayerHelpers::IsFailed(connectedPlayer) && ConnectedPlayerHelpers::WasActiveAtLevelStart(connectedPlayer)) {
            auto playerWorldPosition = MultiplayerPlayerPlacement::GetPlayerWorldPosition(outerCircleRadius, outerCirclePositionAngleForPlayer, layout);
            auto multiplayerConnectedFacade = self->connectedPlayerFactory->Create(connectedPlayer, localPlayerStartState);
            auto t = multiplayerConnectedFacade->get_transform();
            t->set_position(playerWorldPosition);
            t->set_rotation(Quaternion::Euler(0, outerCirclePositionAngleForPlayer, 0));
            self->connectedPlayerControllersMap->set_Item(userId, multiplayerConnectedFacade);
        }
    }
}
