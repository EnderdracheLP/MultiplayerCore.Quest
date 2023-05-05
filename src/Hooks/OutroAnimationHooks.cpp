#include "logging.hpp"
#include "hooking.hpp"

#include "GlobalNamespace/MultiplayerOutroAnimationController.hpp"
#include "GlobalNamespace/MultiplayerResultsPyramidView.hpp"
#include "GlobalNamespace/MultiplayerPlayerResultsData.hpp"
#include "GlobalNamespace/MultiplayerPlayersManager.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"

MAKE_AUTO_HOOK_MATCH(MultiplayerOutroAnimationController_BindRingsAndAudio, &::GlobalNamespace::MultiplayerOutroAnimationController::BindRingsAndAudio, void, GlobalNamespace::MultiplayerOutroAnimationController* self, ArrayW<UnityEngine::GameObject *> rings, bool isMock, bool isDuel, GlobalNamespace::MultiplayerTimelineMock *timelineMock) {
    auto max5Rings = ArrayW<UnityEngine::GameObject*>(il2cpp_array_size_t(std::min(il2cpp_array_size_t(5), rings.size())));
    for (int i = 0; auto& ring : max5Rings) ring = rings[i++];

    MultiplayerOutroAnimationController_BindRingsAndAudio(self, max5Rings, isMock, isDuel, timelineMock);
}

static ListW<GlobalNamespace::IConnectedPlayer*> GetActivePlayersAttacher(ListW<GlobalNamespace::IConnectedPlayer*> allActiveAtGameStartPlayers) {
    // new list
    ListW<GlobalNamespace::IConnectedPlayer*> activePlayers = List<GlobalNamespace::IConnectedPlayer*>::New_ctor();

    // take 4 max
    auto count = std::min(allActiveAtGameStartPlayers.size(), 4);
    for (int i = 0; i < count; i++) activePlayers->Add(allActiveAtGameStartPlayers[i]);

    return activePlayers;
}

MAKE_AUTO_HOOK_MATCH(MultiplayerOutroAnimationController_BindOutroTimeline, &::GlobalNamespace::MultiplayerOutroAnimationController::BindOutroTimeline, void, GlobalNamespace::MultiplayerOutroAnimationController* self) {
    // save original data for restoring later
    auto originalData = self->multiplayerPlayersManager->allActiveAtGameStartPlayers;
    self->multiplayerPlayersManager->allActiveAtGameStartPlayers = GetActivePlayersAttacher(originalData)->i_IReadOnlyList_1_T();

    // call original
    MultiplayerOutroAnimationController_BindOutroTimeline(self);
    // restore original data back to manager
    self->multiplayerPlayersManager->allActiveAtGameStartPlayers = originalData;
}

MAKE_AUTO_HOOK_MATCH(MultiplayerResultsPyramidView_SetupResults, &::GlobalNamespace::MultiplayerResultsPyramidView::SetupResults, void, GlobalNamespace::MultiplayerResultsPyramidView* self, System::Collections::Generic::IReadOnlyList_1<::GlobalNamespace::MultiplayerPlayerResultsData*>* resultsData, UnityEngine::Transform* badgeStartTransform, UnityEngine::Transform* badgeMidTransform) {
    auto resultsDataList = ListW<GlobalNamespace::MultiplayerPlayerResultsData*>(resultsData);
    ListW<GlobalNamespace::MultiplayerPlayerResultsData*> max5Data = List<GlobalNamespace::MultiplayerPlayerResultsData*>::New_ctor();

    auto count = std::min(resultsDataList.size(), 5);
    for (int i = 0; i < count; i++) max5Data->Add(resultsDataList[i]);

    MultiplayerResultsPyramidView_SetupResults(self, max5Data->i_IReadOnlyList_1_T(), badgeStartTransform, badgeMidTransform);
}
