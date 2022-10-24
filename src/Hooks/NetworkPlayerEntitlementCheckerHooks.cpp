#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "Utils/ExtraSongData.hpp"
#include "Utilities.hpp"
#include "Utils/EnumUtils.hpp"
#include "shared/GlobalFields.hpp"

#include "GlobalNamespace/NetworkPlayerEntitlementChecker.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "GlobalNamespace/IMenuRpcManager.hpp"

#include "System/Action_3.hpp"
#include "System/Threading/Tasks/Task_1.hpp"

#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

//Dont think this would cause blackscreen

using namespace GlobalNamespace;
using namespace System::Threading::Tasks;

std::string missingLevelText;

namespace MultiplayerCore {

#pragma region Fields

    std::unordered_map<std::string, std::map<std::string, int>> entitlementDictionary;

#pragma endregion

    // Subscribe this method to 'menuRpcManager.setIsEntitledToLevelEvent' when on NetworkPlayerEntitlementChecker.Start, unsub on destroy
    static void HandleEntitlementReceived(StringW userId, StringW levelId, EntitlementsStatus entitlement) {
        std::string cUserId = static_cast<std::string>(userId);
        std::string cLevelId = static_cast<std::string>(levelId);

        getLogger().debug("[HandleEntitlementReceived] Received Entitlement from user '%s' for level '%s' with status '%s'",
            cUserId.c_str(),
            cLevelId.c_str(),
            EnumUtils::GetEnumName(entitlement).c_str()
            );

        // if (entitlement == EntitlementsStatus::NotOwned && MultiplayerCore::Utils::HasRequirement(RuntimeSongLoader::API::GetLevelById(cLevelId))) {
        //     IConnectedPlayer* player = _multiplayerSessionManager->GetPlayerByUserId(userId);
        //     if (player) {
        //         if (!(player->HasState(getMEStateStr()) || player->HasState(getNEStateStr()) || player->HasState(getChromaStateStr())))
        //             missingLevelText = "One or more players might be missing mod Requirements";
        //     }
        //     else {
        //         missingLevelText = "Error Checking Requirement: Player not found";
        //     }
        // }
        entitlementDictionary[cUserId][cLevelId] = entitlement.value;
    }

#pragma region Hooks
    MAKE_HOOK_MATCH(NetworkPlayerEntitlementChecker_GetEntitlementStatus, &NetworkPlayerEntitlementChecker::GetEntitlementStatus, Task_1<EntitlementsStatus>*, NetworkPlayerEntitlementChecker* self, StringW levelIdCS) {
        missingLevelText.clear();
        std::string levelId = static_cast<std::string>(levelIdCS);
        getLogger().info("NetworkPlayerEntitlementChecker_GetEntitlementStatus: %s", levelId.c_str());
        if (IsCustomLevel(levelId)) {
            if (HasSong(levelId)) {
                getLogger().info("NetworkPlayerEntitlementChecker_GetEntitlementStatus has song, checking requirements");
                auto extraSongData  = MultiplayerCore::Utils::ExtraSongData::FromLevelId(levelId);
                if (!extraSongData)
                    return Task_1<EntitlementsStatus>::New_ctor(EntitlementsStatus::Ok);

                std::vector<std::string> requirements;
                for (auto& diff : extraSongData->_difficulties) {
                    if (diff.additionalDifficultyData)
                    {
                        auto reqVec = diff.additionalDifficultyData->_requirements;
                        std::remove_copy_if(reqVec.begin(), reqVec.end(), back_inserter(requirements), MultiplayerCore::Utilities::Contained<std::string>(requirements));                        
                        // // join
                        // auto mergepoint = requirements.end();

                        // std::copy(reqVec.begin(), reqVec.end(), std::back_inserter(requirements));

                        // // merge
                        // std::inplace_merge(requirements.begin(), mergepoint, requirements.end());

                        // for (auto& req : diff.additionalDifficultyData->_requirements) {
                        //     requirements.push_back(req);
                        // }
                    }
                }
                bool hasRequirements = true;
                std::string tempMissingLevelText = "You or other players might be missing the following mod Requirements:\n";
                for (auto& req : requirements) {
                    if (!RequirementUtils::GetRequirementInstalled(req) && !RequirementUtils::GetIsForcedSuggestion(req))
                    {
                        hasRequirements = false;
                        tempMissingLevelText += req + "\n";
                    }
                }
                if (!hasRequirements) {
                    missingLevelText = tempMissingLevelText;
                    return Task_1<EntitlementsStatus>::New_ctor(EntitlementsStatus::NotOwned);
                }
                return Task_1<EntitlementsStatus>::New_ctor(EntitlementsStatus::Ok);
            }
            else {
                getLogger().info("NetworkPlayerEntitlementChecker_GetEntitlementStatus we don't have the song, falling back to beatsaver compare");
                auto task = Task_1<EntitlementsStatus>::New_ctor();
                BeatSaver::API::GetBeatmapByHashAsync(Utilities::GetHash(levelId),
                    [task, levelId](std::optional<BeatSaver::Beatmap> beatmaps) {
                        QuestUI::MainThreadScheduler::Schedule(
                            [task, beatmaps, levelId] {
                                if (beatmaps.has_value()) {
                                    auto& beatmap = beatmaps->GetVersions().front();
                                    std::string mapHash = beatmap.GetHash();
                                    std::transform(mapHash.begin(), mapHash.end(), mapHash.begin(), toupper);
                                    if (mapHash == Utilities::GetHash(levelId)) {
                                        static std::string tempMissingLevelText = "You or other players might be missing the following mod Requirements:\n";
                                        bool isMissingRequirements = false;
                                        // TODO: I'd like to actually only check the current difficulty, but I still need to figure out how to get the current selected difficulty
                                        for (auto& diff : beatmap.GetDiffs()) {
                                            // if (diff.GetChroma() && !RequirementUtils::GetRequirementInstalled("Chroma") && !RequirementUtils::GetIsForcedSuggestion("Chroma")) {
                                            //     task->TrySetResult(EntitlementsStatus::NotOwned);
                                            //     getLogger().warning("Map contains Chroma difficulty and Chroma doesn't seem to be installed, returning NotOwned");
                                            //     if (tempMissingLevelText.find("Chroma") == std::string::npos) tempMissingLevelText += "Chroma";
                                            //     isMissingRequirements = true;
                                            // }
                                            /*else*/ if (diff.GetNE() && !RequirementUtils::GetRequirementInstalled("Noodle Extensions") && !RequirementUtils::GetIsForcedSuggestion("Noodle Extensions")) {
                                                task->TrySetResult(EntitlementsStatus::NotOwned);
                                                getLogger().warning("Map contains NE difficulty but NoodleExtensions doesn't seem to be installed, returning NotOwned");
                                                if (tempMissingLevelText.find("Noodle Extensions") == std::string::npos) tempMissingLevelText += "Noodle Extensions";
                                                isMissingRequirements = true;
                                            }
                                            else if (diff.GetME() && !RequirementUtils::GetRequirementInstalled("MappingExtensions") && !RequirementUtils::GetIsForcedSuggestion("MappingExtensions")) {
                                                task->TrySetResult(EntitlementsStatus::NotOwned);
                                                getLogger().warning("Map contains ME difficulty but MappingExtensions doesn't seem to be installed, returning NotOwned");
                                                if (tempMissingLevelText.find("Mapping Extensions") == std::string::npos) tempMissingLevelText += "Mapping Extensions";
                                                isMissingRequirements = true;
                                            }
                                        }
                                        if (isMissingRequirements) {
                                            missingLevelText = tempMissingLevelText;
                                            task->TrySetResult(EntitlementsStatus::NotOwned);
                                        }
                                        task->TrySetResult(EntitlementsStatus::NotDownloaded);
                                        return;
                                    }
                                    else {
                                        getLogger().error("Hash returned by BeatSaver doesn't match requested hash: Expected: '%s' got '%s'", Utilities::GetHash(levelId).c_str(), mapHash.c_str());
                                        missingLevelText = string_format("Hash returned by BeatSaver doesn't match requested hash: Expected: '%s' got '%s'", Utilities::GetHash(levelId).c_str(), mapHash.c_str());
                                    }
                                    //}
                                    task->TrySetResult(EntitlementsStatus::NotOwned);
                                }
                                else {
                                    task->TrySetResult(EntitlementsStatus::NotOwned);
                                }
                            }
                        );
                    }
                );
                return task;
            }
        }
        else {
            return NetworkPlayerEntitlementChecker_GetEntitlementStatus(self, levelIdCS);
        }
    }

    System::Action_3<::StringW, ::StringW, EntitlementsStatus>* entitlementAction = nullptr;

    MAKE_HOOK_MATCH(NetworkPlayerEntitlementChecker_Start, &NetworkPlayerEntitlementChecker::Start, void, NetworkPlayerEntitlementChecker* self) {
        getLogger().debug("NetworkPlayerEntitlementChecker_Start");
        if (!entitlementAction) entitlementAction = il2cpp_utils::MakeDelegate<System::Action_3<::StringW, ::StringW, EntitlementsStatus>*>(classof(System::Action_3<::StringW, ::StringW, EntitlementsStatus>*), &HandleEntitlementReceived);
        self->rpcManager->add_setIsEntitledToLevelEvent(entitlementAction);
        NetworkPlayerEntitlementChecker_Start(self);
    }

    MAKE_HOOK_MATCH(NetworkPlayerEntitlementChecker_OnDestroy, &NetworkPlayerEntitlementChecker::OnDestroy, void, NetworkPlayerEntitlementChecker* self) {
        getLogger().debug("NetworkPlayerEntitlementChecker_OnDestroy");
       if (entitlementAction) {
            Utilities::ClearDelegate(entitlementAction);
            entitlementAction = nullptr;
       }
       NetworkPlayerEntitlementChecker_OnDestroy(self);
    }
#pragma endregion

    void Hooks::Early_NetworkPlayerEntitlementChecker() {
        INSTALL_HOOK(getLogger(), NetworkPlayerEntitlementChecker_Start);
    }

    void Hooks::Late_NetworkplayerEntitlementChecker() {
        INSTALL_HOOK_ORIG(getLogger(), NetworkPlayerEntitlementChecker_GetEntitlementStatus);
        INSTALL_HOOK(getLogger(), NetworkPlayerEntitlementChecker_OnDestroy);
    }
}
