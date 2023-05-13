#include "logging.hpp"
#include "hooking.hpp"
#include "Patchers/PlayerCountPatcher.hpp"

#include "GlobalNamespace/CreateServerFormController.hpp"
#include "GlobalNamespace/CreateServerFormData.hpp"
#include "GlobalNamespace/FormattedFloatListSettingsController.hpp"
#include "GlobalNamespace/MultiplayerLobbyController.hpp"
#include "GlobalNamespace/MultiplayerPlayerPlacement.hpp"
#include "GlobalNamespace/MultiplayerLayoutProvider.hpp"
#include "GlobalNamespace/MultiplayerPlayerLayout.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarPlaceManager.hpp"
#include "GlobalNamespace/ILobbyStateDataModel.hpp"
#include "GlobalNamespace/MultiplayerLobbyCenterStageManager.hpp"
#include "GlobalNamespace/MenuEnvironmentManager.hpp"
#include "GlobalNamespace/GameplayServerConfiguration.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarManager.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarController_Factory.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarPlace_Pool.hpp"
#include "System/Action_2.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"

using namespace MultiplayerCore::Patchers;

MAKE_AUTO_HOOK_MATCH(CreateServerFormController_Setup, &::GlobalNamespace::CreateServerFormController::Setup, void, GlobalNamespace::CreateServerFormController* self, int selectedNumberOfPlayers, bool netDiscoverable) {
    auto patcher = PlayerCountPatcher::get_instance();
    auto min = patcher->get_MinPlayers();
    auto max = patcher->get_MaxPlayers();

    DEBUG("Creating server form with player clamp between '{}' and '{}'", min, max);
    selectedNumberOfPlayers = std::clamp(selectedNumberOfPlayers, min, max);

    auto count = max - min + 1;
    ArrayW<float> values = ArrayW<float>(il2cpp_array_size_t(count));
    for (int i = min; auto& v : values) v = i++;

    self->maxPlayersList->set_values(values);
    CreateServerFormController_Setup(self, selectedNumberOfPlayers, netDiscoverable);
}

MAKE_AUTO_HOOK_MATCH(CreateServerFormController_get_formData, &::GlobalNamespace::CreateServerFormController::get_formData, GlobalNamespace::CreateServerFormData, GlobalNamespace::CreateServerFormController* self) {
    auto res = CreateServerFormController_get_formData(self);
    res.maxPlayers = self->maxPlayersList->get_value();
    return res;
}

MAKE_AUTO_HOOK_MATCH(MultiplayerLobbyController_ActivateMultiplayerLobby, &::GlobalNamespace::MultiplayerLobbyController::ActivateMultiplayerLobby, void, GlobalNamespace::MultiplayerLobbyController* self) {
    // fix circle for bigger player counts
    self->innerCircleRadius = 1.0f;
    self->minOuterCircleRadius = 4.4f;
    MultiplayerLobbyController_ActivateMultiplayerLobby(self);

    auto placeManager = self->multiplayerLobbyAvatarPlaceManager;
    // auto menuEnvironmentManager = self->menuEnvironmentManager;
    // auto stageManager = self->multiplayerLobbyCenterStageManager;
    auto lobbyStateDataModel = placeManager->lobbyStateDataModel;

    // auto innerCircleRadius = placeManager->innerCircleRadius;
    // auto minOuterCircleRadius = placeManager->minOuterCircleRadius;
    // DEBUG("innerCircleRadius: '{}', minOuterCircleRadius: '{}'", self->innerCircleRadius, self->innerCircleRadius);

    auto maxPlayerCount = lobbyStateDataModel->get_configuration().maxPlayerCount;
    // auto angleBetweenPlayersWithEvenAdjustment = GlobalNamespace::MultiplayerPlayerPlacement::GetAngleBetweenPlayersWithEvenAdjustment(maxPlayerCount, GlobalNamespace::MultiplayerPlayerLayout::Circle);
    // auto outerCircleRadius = std::max(GlobalNamespace::MultiplayerPlayerPlacement::GetOuterCircleRadius(angleBetweenPlayersWithEvenAdjustment, self->innerCircleRadius), self->innerCircleRadius);
    // DEBUG("maxPlayerCount: '{}', angleBetweenPlayersWithEvenAdjustment: '{}', outerCircleRadius: '{}'", maxPlayerCount, angleBetweenPlayersWithEvenAdjustment, outerCircleRadius);

    static auto SetActiveForTransformInObjectTransform = +[](GlobalNamespace::MenuEnvironmentManager* envManager, StringW name, bool active){
        auto t = envManager->get_transform()->Find(name);
        if (t) t->get_gameObject()->SetActive(active);
        else DEBUG("Unable to set active state '{}' on 'MenuEnvironmentManager/{}', it was not found", active, name);
    };

    bool buildingsEnabled = maxPlayerCount <= 18;
    SetActiveForTransformInObjectTransform(self->menuEnvironmentManager, "MultiplayerLobbyEnvironment/ConstructionL", buildingsEnabled);
    SetActiveForTransformInObjectTransform(self->menuEnvironmentManager, "MultiplayerLobbyEnvironment/ConstructionR", buildingsEnabled);

    // float centerScreenScale = outerCircleRadius / self->minOuterCircleRadius;
    // stageManager->get_transform()->set_localScale({ centerScreenScale, centerScreenScale, centerScreenScale });
}

MAKE_AUTO_HOOK_MATCH(MultiplayerPlayerPlacement_GetAngleBetweenPlayersWithEvenAdjustment, &::GlobalNamespace::MultiplayerPlayerPlacement::GetAngleBetweenPlayersWithEvenAdjustment, float, int numberOfPlayers, GlobalNamespace::MultiplayerPlayerLayout layout) {
    DEBUG("Getting angle between '{}' players with layout '{}'", numberOfPlayers, layout);
    auto patcher = PlayerCountPatcher::get_instance();
    if (!patcher->get_AddEmptyPlayerSlotForEvenCount()) {
        DEBUG("AddEmptyPlayerSlotForEvenCount set to false, returning 360.0f / {} with result {}", numberOfPlayers, 360.0f / (float)numberOfPlayers);
        return 360.0f / (float)numberOfPlayers;
    }
    return MultiplayerPlayerPlacement_GetAngleBetweenPlayersWithEvenAdjustment(numberOfPlayers, layout);
}

// possibly does not call orig
MAKE_AUTO_HOOK_ORIG_MATCH(MultiplayerLayoutProvider_CalculateLayout, &::GlobalNamespace::MultiplayerLayoutProvider::CalculateLayout, GlobalNamespace::MultiplayerPlayerLayout, GlobalNamespace::MultiplayerLayoutProvider* self, int activePlayersCount) {
    auto patcher = PlayerCountPatcher::get_instance();
    if (!patcher->get_AddEmptyPlayerSlotForEvenCount()) {
        DEBUG("AddEmptyPlayerSlotForEvenCount set to false, returning layout for activePlayersCount '{}'", activePlayersCount);
        auto layout = activePlayersCount == 2 ? GlobalNamespace::MultiplayerPlayerLayout::Duel : GlobalNamespace::MultiplayerPlayerLayout::Circle;
        self->set_layout(layout);
        self->set_activePlayerSpotsCount(activePlayersCount);
        DEBUG("Setting layout to '{}' with '{}' players", layout, activePlayersCount);
        auto action = self->playersLayoutWasCalculatedEvent;
        if (action) action->Invoke(layout, activePlayersCount);
        return layout;
    } else {
        return MultiplayerLayoutProvider_CalculateLayout(self, activePlayersCount);
    }

}

// Too lazy to add namespaces
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