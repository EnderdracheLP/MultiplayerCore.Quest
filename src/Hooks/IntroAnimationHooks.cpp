#include "hooking.hpp"
#include "logging.hpp"

#include "custom-types/shared/delegate.hpp"
#include "GlobalNamespace/MultiplayerIntroAnimationController.hpp"
#include "GlobalNamespace/MultiplayerPlayersManager.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "UnityEngine/Timeline/TimelineAsset.hpp"
#include "UnityEngine/Timeline/AudioTrack.hpp"
#include "UnityEngine/Playables/PlayableDirector.hpp"
#include "UnityEngine/Playables/PlayableAsset.hpp"
#include "UnityEngine/GameObject.hpp"
#include "System/Action.hpp"
#include "System/Collections/Generic/IReadOnlyList_1.hpp"
#include "System/Collections/Generic/IReadOnlyCollection_1.hpp"

static UnityEngine::Playables::PlayableDirector* originalDirector;
static int iteration = 0;

MAKE_AUTO_HOOK_MATCH(MultiplayerIntroAnimationController_PlayIntroAnimation, &::GlobalNamespace::MultiplayerIntroAnimationController::PlayIntroAnimation, void, GlobalNamespace::MultiplayerIntroAnimationController* self, float maxDesiredIntroAnimationDuration, System::Action* onCompleted) {
    // prefix
    DEBUG("Creating intro PlayableDirector for iteration '{}'", iteration);
    auto originalDirector = self->_introPlayableDirector;

    if (iteration != 0) {
        auto newPlayableGameObject = UnityEngine::GameObject::New_ctor();
        self->_introPlayableDirector = newPlayableGameObject->AddComponent<UnityEngine::Playables::PlayableDirector*>();

        using PlayableDirector_SetPlayableAsset = function_ptr_t<void, UnityEngine::Playables::PlayableDirector*, UnityEngine::ScriptableObject*>;
        static auto playableDirector_SetPlayableAsset = reinterpret_cast<PlayableDirector_SetPlayableAsset>(il2cpp_functions::resolve_icall("UnityEngine.Playables.PlayableDirector::SetPlayableAsset"));
        playableDirector_SetPlayableAsset(self->_introPlayableDirector, originalDirector->get_playableAsset());

        onCompleted = custom_types::MakeDelegate<System::Action*>(
            std::function<void()>(
                [onCompleted, newPlayableGameObject](){
                    UnityEngine::Object::Destroy(newPlayableGameObject);
                    if (onCompleted) onCompleted->Invoke();
                }
            )
        );
    }

    auto tl = reinterpret_cast<UnityEngine::Timeline::TimelineAsset*>(self->_introPlayableDirector->get_playableAsset());
    ArrayW<UnityEngine::Timeline::TrackAsset*> outputTracks(tl->GetOutputTracks());
    for (auto track : outputTracks) {
        auto audioTrack = il2cpp_utils::try_cast<UnityEngine::Timeline::AudioTrack>(track);
        track->set_muted(audioTrack.has_value() && iteration != 0);
    }

    self->_bindingFinished = false;

    // orig call
    MultiplayerIntroAnimationController_PlayIntroAnimation(self, maxDesiredIntroAnimationDuration, onCompleted);

    // postfix
    iteration++;
    self->_introPlayableDirector = originalDirector;
    auto players = self->_multiplayerPlayersManager->get_allActiveAtGameStartPlayers();
    auto c = static_cast<System::Collections::Generic::IReadOnlyCollection_1<::GlobalNamespace::IConnectedPlayer*>*>(*players)->get_Count();
    int playerCount = 0;
    for (int i = 0; i < c; i++) {
        if (!players->Item[i]->get_isMe())
            playerCount++;
    }

    if (iteration < ((playerCount + 3) / 4)) {
        self->PlayIntroAnimation(maxDesiredIntroAnimationDuration, onCompleted);
    } else iteration = 0;
}

static ListW<GlobalNamespace::IConnectedPlayer*> GetActivePlayersAttacher(ListW<GlobalNamespace::IConnectedPlayer*> allActiveAtGameStartPlayers) {
    // new list
    auto activePlayers = ListW<GlobalNamespace::IConnectedPlayer*>::New();

    // filter for players that are not the local player
    std::vector<GlobalNamespace::IConnectedPlayer*> notMePlayers;
    for (auto p : allActiveAtGameStartPlayers)
        if (!p->get_isMe()) notMePlayers.emplace_back(p);

    // skip(iteration * 4).take(4)
    auto start = std::next(notMePlayers.begin(), iteration * 4);
    auto end = std::next(start, 4);

    for (auto itr = start; itr < end && itr < notMePlayers.end(); itr++) {
        activePlayers->Add(*itr);
    }

    if (iteration == 0) {
        auto me = std::find_if(allActiveAtGameStartPlayers.begin(), allActiveAtGameStartPlayers.end(), [](auto p){ return p->get_isMe(); });
        if (me != allActiveAtGameStartPlayers.end())
            activePlayers->Add(*me);
    }

    return activePlayers;
}

MAKE_AUTO_HOOK_MATCH(MultiplayerIntroAnimationController_BindTimeline, &::GlobalNamespace::MultiplayerIntroAnimationController::BindTimeline, void, GlobalNamespace::MultiplayerIntroAnimationController* self) {
    // the game early returns on this bool. since we also are going to be doing things it's best to also check the same bool for early return
    if (self->_bindingFinished) {
        MultiplayerIntroAnimationController_BindTimeline(self);
        return;
    }

    // save original data for restoring later
    auto originalData = self->_multiplayerPlayersManager->allActiveAtGameStartPlayers;
    self->_multiplayerPlayersManager->_allActiveAtGameStartPlayers = *static_cast<System::Collections::Generic::List_1<::GlobalNamespace::IConnectedPlayer*>*>(GetActivePlayersAttacher(originalData));

    // call original
    MultiplayerIntroAnimationController_BindTimeline(self);
    // restore original data back to manager
    self->_multiplayerPlayersManager->_allActiveAtGameStartPlayers = originalData;
}
