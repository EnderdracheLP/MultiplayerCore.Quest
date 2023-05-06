#include "Objects/MpEntitlementChecker.hpp"
#include "Utils/ExtraSongData.hpp"
#include "Utils/EnumUtils.hpp"
#include "Utilities.hpp"
#include "logging.hpp"

#include "bsml/shared/Helpers/delegates.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"

#include "GlobalNamespace/IMenuRpcManager.hpp"

DEFINE_TYPE(MultiplayerCore::Objects, MpEntitlementChecker);

// Accessing "private" method from pinkcore
namespace RequirementUtils {
    bool GetRequirementInstalled(std::string requirement);
}

namespace MultiplayerCore::Objects {
    void MpEntitlementChecker::ctor() {
        INVOKE_CTOR();
        INVOKE_BASE_CTOR(classof(GlobalNamespace::NetworkPlayerEntitlementChecker*));
    }

    void MpEntitlementChecker::Inject(GlobalNamespace::IMultiplayerSessionManager* sessionManager) {
        _sessionManager = sessionManager;
    }

    void MpEntitlementChecker::Start_override() {
        GlobalNamespace::NetworkPlayerEntitlementChecker::Start();
        rpcManager->add_setIsEntitledToLevelEvent(
            BSML::MakeSystemAction<StringW, StringW, GlobalNamespace::EntitlementsStatus>(this, ___HandleSetIsEntitledToLevel_MethodRegistrator.get())
        );
    }

    void MpEntitlementChecker::OnDestroy_override() {
        GlobalNamespace::NetworkPlayerEntitlementChecker::OnDestroy();
    }

    void MpEntitlementChecker::HandleGetIsEntitledToLevel_override(StringW userId, StringW levelId) {
        GlobalNamespace::NetworkPlayerEntitlementChecker::HandleGetIsEntitledToLevel(userId, levelId);
    }

    void MpEntitlementChecker::HandleSetIsEntitledToLevel(StringW userId, StringW levelId, GlobalNamespace::EntitlementsStatus entitlement) {
        // operator[] auto inserts instances so we can just check like this,
        // and since this method shouldn't ever be called with Unknown, this is a nice way of doing this

        // TODO: check if this hacky way of doing it is fine
        if (_entitlementsDictionary[userId][levelId] != entitlement) DEBUG("Entitlement from '{}' for '{}' is {}", userId, levelId, Utils::EnumUtils::GetEnumName(entitlement));

        _entitlementsDictionary[userId][levelId] = entitlement;

        // make a copy of the event to prevent issues with unsubbing while invoking the event
        auto ev = receivedEntitlementEvent;
        ev.invoke(userId, levelId, entitlement);
    }

    EntitlementsStatusTask* MpEntitlementChecker::GetEntitlementStatus_override(StringW levelId) {
        auto levelHash = Utilities::HashForLevelId(levelId);
        // not custom
        if (levelHash.empty()) {
            DEBUG("Not a custom level, returning base call");
            return NetworkPlayerEntitlementChecker::GetEntitlementStatus(levelId);
        }

        auto task = EntitlementsStatusTask::New_ctor();
        task->m_stateFlags = System::Threading::Tasks::Task::TASK_STATE_STARTED;

        std::async(std::launch::async, [this, task, levelId](){
            auto entitlement = GetEntitlementStatus(levelId);
            DEBUG("Entitlement found for level {}: {}", levelId, Utils::EnumUtils::GetEnumName(entitlement));

            task->TrySetResult(entitlement);
            task->m_stateFlags = System::Threading::Tasks::Task::TASK_STATE_RAN_TO_COMPLETION;
            return entitlement;
        });

        return task;
    }

    std::future<GlobalNamespace::EntitlementsStatus> MpEntitlementChecker::GetEntitlementStatusAsync(std::string levelId) {
        return std::async(std::launch::async, std::bind(&MpEntitlementChecker::GetEntitlementStatus, this, levelId));
    }

    GlobalNamespace::EntitlementsStatus MpEntitlementChecker::GetEntitlementStatus(std::string levelId) {
        auto levelHash = Utilities::HashForLevelId(levelId);
        if (levelHash.empty()) return NetworkPlayerEntitlementChecker::GetEntitlementStatus(levelId)->get_Result();

        // Check if this custom song exists locally
        if (RuntimeSongLoader::API::GetLevelByHash(levelHash).has_value()) {
            auto extraSongData = Utils::ExtraSongData::FromLevelId(levelId);
            if (!extraSongData) return GlobalNamespace::EntitlementsStatus::Ok;

            std::list<std::string> requirements;
            for (const auto& diff : extraSongData->difficulties) {
                if (diff.additionalDifficultyData.has_value()) {
                    for (const auto& req : diff.additionalDifficultyData->requirements) {
                        requirements.emplace_back(req);
                    }
                }
            }

            for (const auto& req : requirements)
                if (!RequirementUtils::GetRequirementInstalled(req))
                    return GlobalNamespace::EntitlementsStatus::NotOwned;

            return GlobalNamespace::EntitlementsStatus::Ok;
        }

        // Check beatsaver for the map
        auto beatmap = BeatSaver::API::GetBeatmapByHash(levelHash);
        if (beatmap.has_value()) {
            auto& versions = beatmap->GetVersions();
            const auto& beatmapVersion = std::find_if(versions.begin(), versions.end(), [&levelHash](const auto& v){
                return v.GetHash() == levelHash;
            });
            if (beatmapVersion == versions.end()) {
                WARNING("Level hash {} was not found in map versions provided by beatsaver!", levelHash);
                return GlobalNamespace::EntitlementsStatus::NotOwned;
            }

            std::list<std::string> requirements;
            for (const auto& diff : beatmapVersion->GetDiffs()) {
                if (diff.GetChroma()) requirements.emplace_back("Chroma");
                if (diff.GetME()) requirements.emplace_back("Mapping Extensions");
                if (diff.GetNE()) requirements.emplace_back("Noodle Extensions");
            }

            for (const auto& req : requirements)
                if (!RequirementUtils::GetRequirementInstalled(req))
                    return GlobalNamespace::EntitlementsStatus::NotOwned;

            return GlobalNamespace::EntitlementsStatus::NotDownloaded;
        }

        WARNING("Level hash {} was not found on beatsaver", levelHash);
        // map didn't exist anywhere, must be WIP or something...
        // either way we don't have it and can't get it
        return GlobalNamespace::EntitlementsStatus::NotOwned;
    }

    GlobalNamespace::EntitlementsStatus MpEntitlementChecker::GetUserEntitlementStatusWithoutRequest(StringW userId, StringW levelId) {
        auto userDict = _entitlementsDictionary.find(userId);
        if (userDict != _entitlementsDictionary.end()) {
            auto currentEntitlement = userDict->second.find(levelId);
            if (currentEntitlement != userDict->second.end())
                return currentEntitlement->second;
        }

        return GlobalNamespace::EntitlementsStatus::Unknown;
    }

}
