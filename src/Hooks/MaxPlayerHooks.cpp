#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "Utils/MethodCache.hpp"
#include "Utilities.hpp"
#include "GlobalNamespace/MultiplayerOutroAnimationController.hpp"
#include "GlobalNamespace/MultiplayerIntroAnimationController.hpp"
#include "GlobalNamespace/CreateServerFormController.hpp"
#include "GlobalNamespace/CreateServerFormData.hpp"
#include "GlobalNamespace/FormattedFloatListSettingsController.hpp"
#include "GlobalNamespace/MultiplayerResultsPyramidView.hpp"
#include "GlobalNamespace/MultiplayerPlayerResultsData.hpp"
#include "GlobalNamespace/MultiplayerPlayersManager.hpp"
#include "GlobalNamespace/MultiplayerLayoutProvider.hpp"
#include "GlobalNamespace/MultiplayerConnectedPlayerIntroAnimator.hpp"
#include "GlobalNamespace/MultiplayerScoreRingManager.hpp"
#include "GlobalNamespace/ConnectedPlayerHelpers.hpp"
#include "GlobalNamespace/MultiplayerLocalActivePlayerIntroAnimator.hpp"
#include "GlobalNamespace/MultiplayerLocalInactivePlayerOutroAnimator.hpp"
#include "GlobalNamespace/MultiplayerLocalInactivePlayerFacade.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Debug.hpp"

#include "System/Action.hpp"

#include "System/Collections/Generic/IReadOnlyList_1.hpp"
#include "System/Collections/Generic/Queue_1.hpp"

#include "UnityEngine/Timeline/TimelineAsset.hpp"
#include "UnityEngine/Timeline/TrackAsset.hpp"
#include "UnityEngine/Timeline/AudioTrack.hpp"

#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

using namespace GlobalNamespace;
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Linq;
using namespace UnityEngine;
using namespace UnityEngine::Playables;
using namespace UnityEngine::Timeline;

namespace MultiplayerCore {

    int targetIterations = 0;

    static ConstString noFacadeError("Neither active or inactive facade exists, this should not happen");

#pragma region IntroAnimationPatch
    MAKE_HOOK_MATCH(IntroAnimationPatch, &MultiplayerIntroAnimationController::PlayIntroAnimation, void, MultiplayerIntroAnimationController* self, float maxDesiredIntroAnimationDuration, Action* onCompleted) {
        getLogger().debug("Creating intro PlayableDirector for iteration '%d'.", targetIterations);
        PlayableDirector* originalDirector = self->introPlayableDirector;
        // if (targetIterations == 0)
        // {
        //     targetIterations = floor((reinterpret_cast<IReadOnlyCollection_1<GlobalNamespace::IConnectedPlayer*>*>(self->multiplayerPlayersManager->allActiveAtGameStartPlayers)->get_Count() - 1) / 4) + 1;
        // }
        try {
            // Create new gameobject to play the animation after first
            if (targetIterations != 0) {
                getLogger().debug("starter animaton");
                // Create duplicated animations
                GameObject* newPlayableGameObject = GameObject::New_ctor();
                self->introPlayableDirector = newPlayableGameObject->AddComponent<PlayableDirector*>();
                MethodCache::SetPlayableAsset(self->introPlayableDirector, originalDirector->get_playableAsset());

                // Cleanup gameobject
                // onCompleted = il2cpp_utils::MakeDelegate<System::Action*>(classof(System::Action*), (std::function<void()>)[self, newPlayableGameObject, &onCompleted] {
                //     GameObject::Destroy(newPlayableGameObject);

                //     // Make sure old action happens by calling it
                //     self->onCompleted->Invoke(); //  TODO: This crashes

                //     QuestUI::MainThreadScheduler::Schedule([onCompleted]{
                //         MultiplayerCore::Utilities::ClearDelegate(onCompleted);
                //     });
                // });
            }

            // Mute audio if animation is not first animation, so audio only plays once
            TimelineAsset* mutedTimeline = reinterpret_cast<TimelineAsset*>(self->introPlayableDirector->get_playableAsset());
            static auto* Enumerable_ToList = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(MethodCache::get_Enumerable_ToList_Generic(), { classof(TrackAsset*) }));
            List<TrackAsset*>* outputTracks = il2cpp_utils::RunMethodRethrow<List_1<TrackAsset*>*, false>(static_cast<Il2CppClass*>(nullptr),
                Enumerable_ToList, mutedTimeline->GetOutputTracks());

            for (int i = 0; i < outputTracks->get_Count(); i++) {
                TrackAsset* currentTrack = outputTracks->get_Item(i);
                bool isAudio = il2cpp_utils::AssignableFrom<AudioTrack*>(reinterpret_cast<Il2CppObject*>(currentTrack)->klass);
                currentTrack->set_muted(isAudio && targetIterations != 0);
            }

            // Makes animator rebind to new playable
            self->bindingFinished = false;
            IntroAnimationPatch(self, maxDesiredIntroAnimationDuration, onCompleted);
            targetIterations++;

            // Reset director to orignal director
            self->introPlayableDirector = originalDirector;

            // Get the list of active players again
            List_1<IConnectedPlayer*>* players = reinterpret_cast<List_1<IConnectedPlayer*>*>(self->multiplayerPlayersManager->get_allActiveAtGameStartPlayers());
            
            // Check that it doesn't contain localplayer
            for (int i = 0; i < players->get_Count(); i++) {
                IConnectedPlayer* currentPlayer = players->get_Item(i);
                if (currentPlayer->get_isMe()) {
                    players->RemoveAt(i);
                }
            }

            // When targetIterations is smaller than the ((players count (without localplayer) + 3) / 4) we run the function again
            if (targetIterations < ((players->get_Count() + 3) / 4)) {
                self->PlayIntroAnimation(maxDesiredIntroAnimationDuration, self->onCompleted);
            } else {
                targetIterations = 0; // Reset
            }
            // targetIterations--;
            // if (targetIterations != 0)
            //     self->PlayIntroAnimation(maxDesiredIntroAnimationDuration, onCompleted);
        }
        catch (il2cpp_utils::exceptions::StackTraceException const& e) {
            // Reset director to real director
            self->introPlayableDirector = originalDirector;
            getLogger().critical("REPORT TO ENDER: Hook IntroAnimationPatch Exception: %s", e.what());
            e.log_backtrace();
            IntroAnimationPatch(self, maxDesiredIntroAnimationDuration, onCompleted);
        }
        catch (il2cpp_utils::RunMethodException const& e) {
            // Reset director to real director
            self->introPlayableDirector = originalDirector;
            getLogger().critical("REPORT TO ENDER: Hook IntroAnimationPatch Exception: %s", e.what());
            e.log_backtrace();
            IntroAnimationPatch(self, maxDesiredIntroAnimationDuration, onCompleted);
        }
        catch (const std::runtime_error& e) {
            // Reset director to real director
            self->introPlayableDirector = originalDirector;
            getLogger().critical("REPORT TO ENDER: Hook IntroAnimationPatch Exception: %s", e.what());
            IntroAnimationPatch(self, maxDesiredIntroAnimationDuration, onCompleted);
        }
    }

    MAKE_HOOK_MATCH(MultiplayerIntroAnimationController_BindTimeline, &MultiplayerIntroAnimationController::BindTimeline, void, MultiplayerIntroAnimationController* self) {
        getLogger().debug("Start: MultiplayerIntroAnimationController_BindTimeline");
        // cpispt = BindTimeline;
        // MultiplayerIntroAnimationController_BindTimeline(self);
        using namespace ::System::Collections::Generic;
        if (self->bindingFinished)
		{
			return;
		}
        static auto* Enumerable_ToList = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(MethodCache::get_Enumerable_ToList_Generic(), { classof(IConnectedPlayer*) }));
        static auto* Enumerable_Take = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(MethodCache::get_Enumerable_Take_Generic(), { classof(IConnectedPlayer*) }));

        List_1<IConnectedPlayer*>* listActivePlayers = il2cpp_utils::RunMethodRethrow<List_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr),
            Enumerable_ToList, reinterpret_cast<IEnumerable_1<IConnectedPlayer*>*>(self->multiplayerPlayersManager->get_allActiveAtGameStartPlayers()));
        IConnectedPlayer* localPlayer = nullptr;

        // Check if active players contains local player and remove local player
        for (int i = 0; i < listActivePlayers->get_Count(); i++) {
            IConnectedPlayer* currentPlayer = listActivePlayers->get_Item(i);
            if (currentPlayer->get_isMe()) {
                listActivePlayers->RemoveAt(i);
                localPlayer = currentPlayer;
            }
        }
        // Skip x amount of players and then take 4
        static auto* Enumerable_Skip_Generic = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(Enumerable*), "Skip", 2));
        static auto* Enumerable_Skip = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(Enumerable_Skip_Generic, { classof(IConnectedPlayer*) }));
        auto* skipResult = il2cpp_utils::RunMethodRethrow<IEnumerable_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr),
            Enumerable_Skip, reinterpret_cast<IEnumerable_1<IConnectedPlayer*>*>(listActivePlayers), targetIterations * 4);

        auto* takeResult = il2cpp_utils::RunMethodRethrow<IEnumerable_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr),
            Enumerable_Take, skipResult, 4);

        List_1<IConnectedPlayer*>* selectedActivePlayers = il2cpp_utils::RunMethodRethrow<List_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr),
            Enumerable_ToList, takeResult);

        // Add back local player if not null
        if (targetIterations == 0 && localPlayer != nullptr) {
            selectedActivePlayers->Add(localPlayer);
        }

        //set list of players
        IReadOnlyList_1<::GlobalNamespace::IConnectedPlayer*>* allActiveAtGameStartPlayers = reinterpret_cast<IReadOnlyList_1<IConnectedPlayer*>*>(selectedActivePlayers);

        // MpCore PC reference
        // IEnumerable<IConnectedPlayer> players = contract.allActiveAtGameStartPlayers.Where(p => !p.isMe);
        // players = players.Skip(_iteration * 4).Take(4);
        // if (_iteration == 0 && contract.allActiveAtGameStartPlayers.Any(p => p.isMe))
        //     players.Append(contract.allActiveAtGameStartPlayers.First(p => p.isMe));
        // return players.ToList();		
        
        // For fucks sake, all the above and the below is just to set a different value for 'allActiveAtGameStartPlayers', I hate quest modding
        // IReadOnlyList_1<::GlobalNamespace::IConnectedPlayer*>* allActiveAtGameStartPlayers = self->multiplayerPlayersManager->get_allActiveAtGameStartPlayers();
		Queue_1<int>* queue = self->CalculatePlayerIndexSequence(allActiveAtGameStartPlayers);
		int count = queue->get_Count();
		bool flag = self->layoutProvider->get_layout() == MultiplayerPlayerLayout::Duel;
		float num = self->spawnDuration / std::max<float>(count - 1, 1);
		ArrayW<float> array = ArrayW<float>((il2cpp_array_size_t)count + 1);
		for (int i = 0; i < array.Length(); i++)
		{
			array[i] = self->firstConnectedPlayerStart + (float)i * num;
		}
		ArrayW<GameObject*> array2 = ArrayW<GameObject*>((il2cpp_array_size_t)count);
		int num2 = 0;
		while (queue->get_Count() > 0)
		{
			int index = queue->Dequeue();
			StringW userId = allActiveAtGameStartPlayers->get_Item(index)->get_userId();
			GlobalNamespace::MultiplayerConnectedPlayerFacade*  multiplayerConnectedPlayerFacade;
			if (!self->multiplayerPlayersManager->TryGetConnectedPlayerController(userId, byref(multiplayerConnectedPlayerFacade)))
			{
                StringW message = (self->multiplayerSessionManager->get_localPlayer()->get_userId() == userId) ? "Tried to animate local active player as connected player" : "Unable to find ConnectedPlayerController for userId \"" + userId + "\", skipping animation";
                getLogger().WithContext("UnityEngine::Debug::LogWarning").warning("%s", static_cast<std::string>(message).c_str());
				Debug::LogWarning(((Il2CppObject*)message.convert()));
			}
			else
			{
				GameObject* gameObject = multiplayerConnectedPlayerFacade->get_introAnimator()->get_gameObject();
				self->introPlayableDirector->SetReferenceValue(self->playerTimelinePropertyNames[num2], gameObject);
				array2[num2] = (flag ? nullptr : self->scoreRingManager->GetScoreRingItem(userId)->get_gameObject());
				num2++;
			}
		}
		GameObject* localRing = ((flag || !ConnectedPlayerHelpers::WasActiveAtLevelStart(self->multiplayerSessionManager->get_localPlayer())) ? nullptr : self->scoreRingManager->GetScoreRingItem(self->multiplayerSessionManager->get_localPlayer()->get_userId())->get_gameObject());
		self->BindRingsAndSetTiming(count, array2, localRing);
		if (self->multiplayerPlayersManager->get_inactivePlayerFacade() != nullptr)
		{
			self->introPlayableDirector->SetReferenceValue(self->localPlayerTimelinePropertyName, self->multiplayerPlayersManager->get_inactivePlayerFacade()->get_introAnimator());
		}
		else if (self->multiplayerPlayersManager->get_activeLocalPlayerFacade() != nullptr)
		{
			self->introPlayableDirector->SetReferenceValue(self->localPlayerTimelinePropertyName, self->multiplayerPlayersManager->get_activeLocalPlayerFacade()->get_introAnimator()->get_gameObject());
		}
		else
		{
            getLogger().WithContext("Debug::LogError").error("Neither active or inactive facade exists, this should not happen");
			Debug::LogError(((Il2CppObject*)(Il2CppString*)noFacadeError));
		}
		self->bindingFinished = true;
    }
#pragma endregion

#pragma region OutroAnimationPatch

    MAKE_HOOK_MATCH(MultiplayerOutroAnimationController_BindRingsAndAudio, &MultiplayerOutroAnimationController::BindRingsAndAudio, void, MultiplayerOutroAnimationController* self, ::ArrayW<::UnityEngine::GameObject*> rings, bool isMock, bool isDuel, ::GlobalNamespace::MultiplayerTimelineMock* timelineMock) {
        static auto* Enumerable_Take = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(MethodCache::get_Enumerable_Take_Generic(), { classof(::UnityEngine::GameObject*) }));
        auto* takeResult = il2cpp_utils::RunMethodRethrow<IEnumerable_1<::UnityEngine::GameObject*>*, false>(static_cast<Il2CppClass*>(nullptr),
            Enumerable_Take, reinterpret_cast<IEnumerable_1<::UnityEngine::GameObject*>*>(static_cast<Il2CppArray*>(rings)), 5);

        static auto* Enumerable_ToArray = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(MethodCache::get_Enumerable_ToArray_Generic(), { classof(::UnityEngine::GameObject*) }));
        rings = il2cpp_utils::RunMethodRethrow<::Array<::UnityEngine::GameObject*>*, false>(static_cast<Il2CppClass*>(nullptr),
                Enumerable_ToArray, takeResult);
        MultiplayerOutroAnimationController_BindRingsAndAudio(self, rings, isMock, isDuel, timelineMock);
    }

    MAKE_HOOK_MATCH(MultiplayerOutroAnimationController_BindOutroTimeline, &MultiplayerOutroAnimationController::BindOutroTimeline, void, MultiplayerOutroAnimationController* self) {
        getLogger().debug("Start: MultiplayerOutroAnimationController_BindOutroTimeline");
        static auto* Enumerable_ToList = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(MethodCache::get_Enumerable_ToList_Generic(), { classof(IConnectedPlayer*) }));
        static auto* Enumerable_Take = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(MethodCache::get_Enumerable_Take_Generic(), { classof(IConnectedPlayer*) }));

        auto* result = il2cpp_utils::RunMethodRethrow<IEnumerable_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr),
                Enumerable_Take, reinterpret_cast<List_1<IConnectedPlayer*>*>(self->multiplayerPlayersManager->get_allActiveAtGameStartPlayers()), 4);
        
        List_1<IConnectedPlayer*>* allActiveAtGameStartPlayers = il2cpp_utils::RunMethodRethrow<List_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr),
                Enumerable_ToList, result);

        // Method basegame would call, actually it would be 'IEnumerable_1<IConnectedPlayer*>* allActiveAtGameStartPlayers' but there's no reason for it to be an 'IEnumerable' here
		// List_1<IConnectedPlayer*>* allActiveAtGameStartPlayers = reinterpret_cast<List_1<IConnectedPlayer*>*>(self->multiplayerPlayersManager->get_allActiveAtGameStartPlayers());
		
        if (self->multiplayerPlayersManager->get_inactivePlayerFacade() != nullptr)
		{
			self->outroPlayableDirector->SetReferenceValue(self->localPlayerTimelinePropertyName, self->multiplayerPlayersManager->get_inactivePlayerFacade()->get_outroAnimator()->get_gameObject());
		}
		else if (self->multiplayerPlayersManager->get_activeLocalPlayerFacade() != nullptr)
		{
			self->outroPlayableDirector->SetReferenceValue(self->localPlayerTimelinePropertyName, self->multiplayerPlayersManager->get_activeLocalPlayerFacade()->get_outroAnimator());
		}
		else
		{
            getLogger().WithContext("Debug::LogError").error("Neither active or inactive facade exists, this should not happen");
			Debug::LogError(((Il2CppObject*)(Il2CppString*)noFacadeError));

		}
        // Optimize this part, its a foreach in basegame, we use a for loop, possibly can change usage of num
		int num = 0;
        for (int i = 0; i < allActiveAtGameStartPlayers->get_Count(); i++)
		{
            IConnectedPlayer* connectedPlayer = allActiveAtGameStartPlayers->get_Item(i);
			StringW userId = connectedPlayer->get_userId();
			MultiplayerConnectedPlayerFacade* multiplayerConnectedPlayerFacade;
			if (!self->multiplayerPlayersManager->TryGetConnectedPlayerController(userId, byref(multiplayerConnectedPlayerFacade)))
			{
				if (self->multiplayerSessionManager->get_localPlayer()->get_userId() != userId)
				{
					Debug::LogWarning("Unable to find ConnectedPlayerController for userId \"" + userId + "\", skipping outro animation for this player");
				}
			}
			else
			{
				self->outroPlayableDirector->SetReferenceValue(self->playerTimelinePropertyNames[num], multiplayerConnectedPlayerFacade->get_outroAnimator());
				multiplayerConnectedPlayerFacade->HideBigAvatar();
				num++;
			}
		}
    }

    MAKE_HOOK_MATCH_NO_CATCH(MultiplayerResultsPyramidPatch, &MultiplayerResultsPyramidView::SetupResults, void, MultiplayerResultsPyramidView* self, IReadOnlyList_1<MultiplayerPlayerResultsData*>* resultsData, UnityEngine::Transform* badgeStartTransform, UnityEngine::Transform* badgeMidTransform) {
        try {
            static auto* Enumerable_Take = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(MethodCache::get_Enumerable_Take_Generic(), { classof(MultiplayerPlayerResultsData*) }));
            auto* takeResult = il2cpp_utils::RunMethodRethrow<IEnumerable_1<MultiplayerPlayerResultsData*>*, false>(static_cast<Il2CppClass*>(nullptr),
                Enumerable_Take, reinterpret_cast<IEnumerable_1<MultiplayerPlayerResultsData*>*>(resultsData), 5);

            static auto* Enumerable_ToList = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(MethodCache::get_Enumerable_ToList_Generic(), { classof(MultiplayerPlayerResultsData*) }));
            List<MultiplayerPlayerResultsData*>* newResultsData = il2cpp_utils::RunMethodRethrow<List_1<MultiplayerPlayerResultsData*>*, false>(static_cast<Il2CppClass*>(nullptr),
                Enumerable_ToList, takeResult);

            MultiplayerResultsPyramidPatch(self, (IReadOnlyList_1<MultiplayerPlayerResultsData*>*)newResultsData, badgeStartTransform, badgeMidTransform);
        }
        catch (il2cpp_utils::exceptions::StackTraceException const& e) {
            getLogger().critical("REPORT TO ENDER: Hook MultiplayerResultsPyramidPatch File " __FILE__ " at Line %d: %s", __LINE__, e.what());
            getLogger().debug("returning usual results pyramid");
            e.log_backtrace();
            MultiplayerResultsPyramidPatch(self, resultsData, badgeStartTransform, badgeMidTransform);
        }
        catch (il2cpp_utils::RunMethodException const& e) {
            getLogger().critical("REPORT TO ENDER: Hook MultiplayerResultsPyramidPatch File " __FILE__ " at Line %d: %s", __LINE__, e.what());
            getLogger().debug("returning usual results pyramid");
            e.log_backtrace();
            MultiplayerResultsPyramidPatch(self, resultsData, badgeStartTransform, badgeMidTransform);
        }
        catch (const std::runtime_error& e) {
            getLogger().critical("REPORT TO ENDER: Hook MultiplayerResultsPyramidPatch File " __FILE__ " at Line %d: %s", __LINE__, e.what());
            getLogger().debug("returning usual results pyramid");
            MultiplayerResultsPyramidPatch(self, resultsData, badgeStartTransform, badgeMidTransform);
        }
    }

    MAKE_HOOK_MATCH(CreateServerFormController_get_formData, &CreateServerFormController::get_formData, CreateServerFormData, CreateServerFormController* self) {
        CreateServerFormData result = CreateServerFormController_get_formData(self);
        result.maxPlayers = std::clamp<int>(self->maxPlayersList->get_value(), 2, std::clamp(getConfig().config["MaxPlayers"].GetInt(), 2, 120));
        return result;
    }

    MAKE_HOOK_MATCH_NO_CATCH(CreateServerFormController_Setup, &CreateServerFormController::Setup, void, CreateServerFormController* self, int selectedNumberOfPlayers, bool netDiscoverable) {
        try {
            std::vector<int> rangeVec;
            // We have to manually find the method as the codegen call fails to find the method for this
            static auto* Enumerable_ToArray = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(MethodCache::get_Enumerable_ToArray_Generic(), { classof(int) }));
            il2cpp_utils::RunMethodRethrow<::Array<int>*, false>(static_cast<Il2CppClass*>(nullptr),
                Enumerable_ToArray, Enumerable::Range(2, std::clamp(getConfig().config["MaxPlayers"].GetInt(), 2, 120) - 1))->copy_to(rangeVec);

            // Codegen version of the above, does not work and will cause an Exception due to FindMethod failing
            // Enumerable::ToArray(Enumerable::Range(2, std::clamp(getConfig().config["MaxPlayers"].GetInt(), 2, 120) - 1))->copy_to(rangeVec);

            // Convert our int vector to a float vector
            std::vector<float> resultVec(rangeVec.begin(), rangeVec.end());
            // Convert our vector<float> to an Array<float>*
            self->maxPlayersList->values = il2cpp_utils::vectorToArray(resultVec);
        }
        catch (il2cpp_utils::exceptions::StackTraceException const& e) {
            getLogger().error("REPORT TO ENDER: Hook CreateServerFormController_Setup Exception: %s", e.what());
            e.log_backtrace();
        }
        catch (il2cpp_utils::RunMethodException const& e) {
            getLogger().error("REPORT TO ENDER: Hook CreateServerFormController_Setup Exception: %s", e.what());
            e.log_backtrace();
        }
        catch (std::runtime_error const& e) {
            getLogger().error("REPORT TO ENDER: Hook CreateServerFormController_Setup Exception: %s", e.what());
        }
        CreateServerFormController_Setup(self, selectedNumberOfPlayers, netDiscoverable);
    }

    void Hooks::MaxPlayerHooks() {
        INSTALL_HOOK(getLogger(), MultiplayerResultsPyramidPatch);
        INSTALL_HOOK(getLogger(), IntroAnimationPatch);
        INSTALL_HOOK_ORIG(getLogger(), MultiplayerIntroAnimationController_BindTimeline);
        INSTALL_HOOK_ORIG(getLogger(), MultiplayerOutroAnimationController_BindOutroTimeline);

        INSTALL_HOOK(getLogger(), MultiplayerOutroAnimationController_BindRingsAndAudio);
        INSTALL_HOOK(getLogger(), CreateServerFormController_get_formData);
        INSTALL_HOOK(getLogger(), CreateServerFormController_Setup);

    }
}