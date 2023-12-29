#include "Objects/MpEntitlementChecker.hpp"
#include "Utils/ExtraSongData.hpp"
#include "Utils/EnumUtils.hpp"
#include "Utilities.hpp"
#include "logging.hpp"

#include "lapiz/shared/utilities/MainThreadScheduler.hpp"
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
        _rpcManager->add_setIsEntitledToLevelEvent(
            BSML::MakeSystemAction<StringW, StringW, GlobalNamespace::EntitlementsStatus>(this, ___HandleSetIsEntitledToLevel_MethodRegistrator.get())
        );
    }

    void MpEntitlementChecker::OnDestroy_override() {
        GlobalNamespace::NetworkPlayerEntitlementChecker::OnDestroy();
    }

    void MpEntitlementChecker::HandleGetIsEntitledToLevel_override(StringW userId, StringW levelId) {
        GlobalNamespace::NetworkPlayerEntitlementChecker::HandleGetIsEntitledToLevel(userId, levelId);
    }

    static inline std::string_view EntitlementName(GlobalNamespace::EntitlementsStatus entitlement) {
        switch (entitlement) {
            case GlobalNamespace::EntitlementsStatus::Unknown:
                return "Unknown";
            case GlobalNamespace::EntitlementsStatus::NotOwned:
                return "NotOwned";
            case GlobalNamespace::EntitlementsStatus::NotDownloaded:
                return "NotDownloaded";
            case GlobalNamespace::EntitlementsStatus::Ok:
                return "Ok";
            default:
                return "Null";
        }
    }

    void MpEntitlementChecker::HandleSetIsEntitledToLevel(StringW userId, StringW levelId, GlobalNamespace::EntitlementsStatus entitlement) {
        // operator[] auto inserts instances so we can just check like this,
        // and since this method shouldn't ever be called with Unknown, this is a nice way of doing this

        if (_entitlementsDictionary[userId][levelId] != entitlement || entitlement == GlobalNamespace::EntitlementsStatus::Unknown)
            DEBUG("Entitlement from '{}' for '{}' is {}", userId, levelId, EntitlementName(entitlement));

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

        auto existingTask = _entitlementsTasks.find(levelId);
        if (existingTask != _entitlementsTasks.end()) {
            return existingTask->second.ptr();
        }

        auto task = EntitlementsStatusTask::New_ctor(GlobalNamespace::EntitlementsStatus::NotOwned);
        task->m_stateFlags = System::Threading::Tasks::Task::TASK_STATE_STARTED;
        _entitlementsTasks[levelId] = task;

        std::thread([this, task, levelId](){
            auto entitlement = GetEntitlementStatus(levelId);
            DEBUG("Entitlement found for level {}: {}", levelId, EntitlementName(entitlement));

            // TODO: check if this is really the case or if it was something else
            // don't ask me why but not setting the result on main thread crashes
            Lapiz::Utilities::MainThreadScheduler::Schedule([task, entitlement] {
                DEBUG("Set result: {}", task->TrySetResult(entitlement));
                task->___m_result = entitlement;
                task->m_stateFlags = System::Threading::Tasks::Task::TASK_STATE_RAN_TO_COMPLETION;
            });
        }).detach();

        return task;
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
                auto toCheckHash = v.GetHash();
                if (toCheckHash.size() != levelHash.size()) return false;
                auto toCheckItr = toCheckHash.c_str();
                auto levelHashItr = levelHash.c_str();
                auto toCheckEnd = toCheckItr + toCheckHash.size();

                for (;toCheckItr < toCheckEnd; toCheckItr++, levelHashItr++) {
                    if (tolower(*toCheckItr) != tolower(*levelHashItr)) return false;
                }
                return true;
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
