#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "Utils/CustomData.hpp"
#include "Utilities.hpp"
#include "CS_DataStore.hpp"

//Dont think this would cause blackscreen

using namespace GlobalNamespace;
using namespace System::Threading::Tasks;

std::string missingLevelText;

namespace MultiplayerCore {

#pragma region Fields

    std::map<std::string, std::map<std::string, int>> entitlementDictionary;

#pragma endregion

    // For debugging purposes
    const char* entitlementText(EntitlementsStatus entitlement) {
        switch (entitlement.value) {
        case EntitlementsStatus::Unknown:
            return "Unknown";
        case EntitlementsStatus::NotOwned:
            return "NotOwned";
        case EntitlementsStatus::NotDownloaded:
            return "NotDownloaded";
        case EntitlementsStatus::Ok:
            return "Ok";
        }
        return "";
    }

    // Subscribe this method to 'menuRpcManager.setIsEntitledToLevelEvent' when on NetworkPlayerEntitlementChecker.Start, unsub on destroy
    static void HandleEntitlementReceived(StringW userId, StringW levelId, EntitlementsStatus entitlement) {
        std::string cUserId = static_cast<std::string>(userId);
        std::string cLevelId = static_cast<std::string>(levelId);

        getLogger().debug("[HandleEntitlementReceived] Received Entitlement from user '%s' for level '%s' with status '%s'",
            cUserId.c_str(),
            cLevelId.c_str(),
            entitlementText(entitlement)
            );

        if (entitlement == EntitlementsStatus::NotOwned && MultiplayerCore::Utils::HasRequirement(RuntimeSongLoader::API::GetLevelById(cLevelId))) {
            IConnectedPlayer* player = _multiplayerSessionManager->GetPlayerByUserId(userId);
            if (player) {
                if (!(player->HasState(getMEStateStr()) || player->HasState(getNEStateStr()) || player->HasState(getChromaStateStr())))
                    missingLevelText = "One or more players might be missing mod Requirements";
            }
            else {
                missingLevelText = "Error Checking Requirement: Player not found";
            }
        }
        entitlementDictionary[cUserId][cLevelId] = entitlement.value;
    }

#pragma region Hooks
    MAKE_HOOK_MATCH(NetworkPlayerEntitlementChecker_GetEntitlementStatus, &NetworkPlayerEntitlementChecker::GetEntitlementStatus, Task_1<EntitlementsStatus>*, NetworkPlayerEntitlementChecker* self, StringW levelIdCS) {
        missingLevelText.clear();
        std::string levelId = static_cast<std::string>(levelIdCS);
        getLogger().info("NetworkPlayerEntitlementChecker_GetEntitlementStatus: %s", levelId.c_str());
        if (IsCustomLevel(levelId)) {
            if (HasSong(levelId)) {
                if (MultiplayerCore::Utils::HasRequirement(RuntimeSongLoader::API::GetLevelById(levelId)))
                    return Task_1<EntitlementsStatus>::New_ctor(EntitlementsStatus::Ok);
                else return Task_1<EntitlementsStatus>::New_ctor(EntitlementsStatus::NotOwned);
            }
            else {
                auto task = Task_1<EntitlementsStatus>::New_ctor();
                BeatSaver::API::GetBeatmapByHashAsync(Utilities::GetHash(levelId),
                    [task, levelId](std::optional<BeatSaver::Beatmap> beatmaps) {
                        QuestUI::MainThreadScheduler::Schedule(
                            [task, beatmaps, levelId] {
                                if (beatmaps.has_value()) {
                                    // TODO: Server side check, possibly something better mod side as well, this would just prevent downloading
                                    // Possibly taking a look at this https://github.com/BSMGPink/PinkCore/blob/master/include/Utils/RequirementUtils.hpp
                                    //for (auto& beatmap : beatmaps->GetVersions()) {
                                    auto& beatmap = beatmaps->GetVersions().front();
                                    std::string mapHash = beatmap.GetHash();
                                    std::transform(mapHash.begin(), mapHash.end(), mapHash.begin(), toupper);
                                    if (mapHash == Utilities::GetHash(levelId)) {
                                        for (auto& diff : beatmap.GetDiffs()) {
                                            if (diff.GetChroma() && !AllPlayersHaveChroma() && Modloader::getMods().find("Chroma") != Modloader::getMods().end()) {
                                                task->TrySetResult(EntitlementsStatus::NotOwned);
                                                getLogger().warning("Map contains Chroma difficulty and Chroma is installed, returning NotOwned as Chroma currently causes issues in Multiplayer");
                                                missingLevelText = "Chroma Requirement block, please uninstall Chroma";
                                                return;
                                            }
                                            else if (diff.GetNE() && !AllPlayersHaveNE() && Modloader::getMods().find("NoodleExtensions") == Modloader::getMods().end()) {
                                                task->TrySetResult(EntitlementsStatus::NotOwned);
                                                getLogger().warning("Map contains NE difficulty but NoodleExtensions doesn't seem to be installed, returning NotOwned");
                                                missingLevelText = "You or another Player is Missing the following Requirement: Noodle Extensions";
                                                return;
                                            }
                                            else if (diff.GetME() && !AllPlayersHaveME() && Modloader::getMods().find("MappingExtensions") == Modloader::getMods().end()) {
                                                task->TrySetResult(EntitlementsStatus::NotOwned);
                                                getLogger().warning("Map contains ME difficulty but MappingExtensions doesn't seem to be installed, returning NotOwned");
                                                missingLevelText = "You or another Player is Missing the following Requirement: Mapping Extensions";
                                                return;
                                            }
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
        if (!entitlementAction) entitlementAction = il2cpp_utils::MakeDelegate<System::Action_3<::StringW, ::StringW, EntitlementsStatus>*>(classof(System::Action_3<::StringW, ::StringW, EntitlementsStatus>*), &HandleEntitlementReceived);
        self->rpcManager->add_setIsEntitledToLevelEvent(
        entitlementAction
        );
        NetworkPlayerEntitlementChecker_Start(self);
    }

    // TODO: Figure this part out
    MAKE_HOOK_MATCH(NetworkPlayerEntitlementChecker_OnDestroy, &NetworkPlayerEntitlementChecker::OnDestroy, void, NetworkPlayerEntitlementChecker* self) {
       if (entitlementAction) {
            il2cpp_utils::ClearDelegate({(Il2CppMethodPointer)reinterpret_cast<Il2CppDelegate*>(entitlementAction)->method_ptr.ToPointer(), true});
            entitlementAction = nullptr;
       }
       NetworkPlayerEntitlementChecker_OnDestroy(self);
    }
#pragma endregion

    void Hooks::NetworkplayerEntitlementChecker() {
        INSTALL_HOOK_ORIG(getLogger(), NetworkPlayerEntitlementChecker_GetEntitlementStatus);
        INSTALL_HOOK(getLogger(), NetworkPlayerEntitlementChecker_Start);
        INSTALL_HOOK(getLogger(), NetworkPlayerEntitlementChecker_OnDestroy);
    }
}
