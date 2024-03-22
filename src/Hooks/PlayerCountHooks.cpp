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
#include "System/Action_2.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Vector3.hpp"
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

    self->_maxPlayersList->set_values(values);
    CreateServerFormController_Setup(self, selectedNumberOfPlayers, netDiscoverable);
}

MAKE_AUTO_HOOK_MATCH(CreateServerFormController_get_formData, &::GlobalNamespace::CreateServerFormController::get_formData, GlobalNamespace::CreateServerFormData, GlobalNamespace::CreateServerFormController* self) {
    auto res = CreateServerFormController_get_formData(self);
    res.maxPlayers = self->_maxPlayersList->get_value();
    return res;
}

MAKE_AUTO_HOOK_MATCH(MultiplayerLobbyController_ActivateMultiplayerLobby, &::GlobalNamespace::MultiplayerLobbyController::ActivateMultiplayerLobby, void, GlobalNamespace::MultiplayerLobbyController* self) {
    // fix circle for bigger player counts
    self->_innerCircleRadius = 1.0f;
    self->_minOuterCircleRadius = 4.4f;
    MultiplayerLobbyController_ActivateMultiplayerLobby(self);


    auto placeManager = self->_multiplayerLobbyCenterStageManager;
    auto stageManager = self->_multiplayerLobbyAvatarPlaceManager;
    auto lobbyStateDataModel = placeManager->_lobbyStateDataModel;

    auto maxPlayerCount = lobbyStateDataModel->get_configuration().maxPlayerCount;
    auto angleBetweenPlayersWithEvenAdjustment = GlobalNamespace::MultiplayerPlayerPlacement::GetAngleBetweenPlayersWithEvenAdjustment(maxPlayerCount, GlobalNamespace::MultiplayerPlayerLayout::Circle);
    auto outerCircleRadius = std::max(GlobalNamespace::MultiplayerPlayerPlacement::GetOuterCircleRadius(angleBetweenPlayersWithEvenAdjustment, self->_innerCircleRadius), self->_innerCircleRadius);

    static auto SetActiveForTransformInObjectTransform = +[](GlobalNamespace::MenuEnvironmentManager* envManager, StringW name, bool active){
        auto t = envManager->get_transform()->Find(name);
        if (t) t->get_gameObject()->SetActive(active);
        else DEBUG("Unable to set active state '{}' on 'MenuEnvironmentManager/{}', it was not found", active, name);
    };

    bool buildingsEnabled = maxPlayerCount <= 18;
    SetActiveForTransformInObjectTransform(self->_menuEnvironmentManager, "MultiplayerLobbyEnvironment/ConstructionL", buildingsEnabled);
    SetActiveForTransformInObjectTransform(self->_menuEnvironmentManager, "MultiplayerLobbyEnvironment/ConstructionR", buildingsEnabled);

    float centerScreenScale = outerCircleRadius / self->_minOuterCircleRadius;
    stageManager->get_transform()->set_localScale({ centerScreenScale, centerScreenScale, centerScreenScale });
}

MAKE_AUTO_HOOK_MATCH(MultiplayerPlayerPlacement_GetAngleBetweenPlayersWithEvenAdjustment, &::GlobalNamespace::MultiplayerPlayerPlacement::GetAngleBetweenPlayersWithEvenAdjustment, float, int numberOfPlayers, GlobalNamespace::MultiplayerPlayerLayout layout) {
    auto patcher = PlayerCountPatcher::get_instance();
    if (!patcher->get_AddEmptyPlayerSlotForEvenCount()) {
        return 360.0f / (float)numberOfPlayers;
    }
    return MultiplayerPlayerPlacement_GetAngleBetweenPlayersWithEvenAdjustment(numberOfPlayers, layout);
}

// possibly does not call orig
MAKE_AUTO_HOOK_ORIG_MATCH(MultiplayerLayoutProvider_CalculateLayout, &::GlobalNamespace::MultiplayerLayoutProvider::CalculateLayout, GlobalNamespace::MultiplayerPlayerLayout, GlobalNamespace::MultiplayerLayoutProvider* self, int activePlayersCount) {
    auto patcher = PlayerCountPatcher::get_instance();
    if (!patcher->get_AddEmptyPlayerSlotForEvenCount()) {
        auto layout = activePlayersCount == 2 ? GlobalNamespace::MultiplayerPlayerLayout::Duel : GlobalNamespace::MultiplayerPlayerLayout::Circle;
        self->set_layout(layout);
        self->set_activePlayerSpotsCount(activePlayersCount);
        auto action = self->playersLayoutWasCalculatedEvent;
        if (action) action->Invoke(layout, activePlayersCount);
        return layout;
    } else {
        return MultiplayerLayoutProvider_CalculateLayout(self, activePlayersCount);
    }

}
