#include "Objects/MpEntitlementChecker.hpp"
#include "Utils/ExtraSongData.hpp"
#include "Utils/EnumUtils.hpp"
#include "logging.hpp"
#include "tasks.hpp"
#include "Utilities.hpp"

#include "lapiz/shared/utilities/MainThreadScheduler.hpp"
#include "bsml/shared/Helpers/delegates.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"
#include "songcore/shared/SongLoader/RuntimeSongLoader.hpp"

#include "GlobalNamespace/IMenuRpcManager.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"

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

        receivedEntitlementEvent.invoke(userId, levelId, entitlement);
    }

    EntitlementsStatusTask* MpEntitlementChecker::GetEntitlementStatus_override(StringW levelId) {
        std::string levelHash(HashFromLevelID(levelId));
        // not custom
        if (levelHash.empty()) {
            DEBUG("Not a custom level, returning base call");
            return NetworkPlayerEntitlementChecker::GetEntitlementStatus(levelId);
        }

        if (auto existingTask = _entitlementsTasks.find(levelId); existingTask != _entitlementsTasks.end()) {
            return existingTask->second.ptr();
        }

        auto task = StartTask<GlobalNamespace::EntitlementsStatus>([this, levelId](){
            auto entitlement = GetEntitlementStatus(levelId);
            DEBUG("Entitlement found for level {}: {}", levelId, EntitlementName(entitlement));
            _entitlementsDictionary[_sessionManager->localPlayer->userId][levelId] = entitlement;
            return entitlement;
        });

        _entitlementsTasks[levelId] = task;
        return task;
    }

    GlobalNamespace::EntitlementsStatus MpEntitlementChecker::GetEntitlementStatus(std::string levelId) {
        auto levelHash = HashFromLevelID(std::string_view(levelId));
        if (levelHash.empty()) return NetworkPlayerEntitlementChecker::GetEntitlementStatus(levelId)->get_Result();
        DEBUG("Custom level levelhash: {}", levelHash);
        // Check if this custom song exists locally
        auto level = SongCore::API::Loading::GetLevelByHash(levelHash);
        if (level) {
            auto extraSongData = Utils::ExtraSongData::FromSaveData(level->standardLevelInfoSaveData);
            if (!extraSongData.has_value()) return GlobalNamespace::EntitlementsStatus::Ok;

            std::list<std::string> requirements;
            for (const auto& diff : extraSongData->difficulties) {
                if (diff.additionalDifficultyData.has_value()) {
                    for (const auto& req : diff.additionalDifficultyData->requirements) {
                        requirements.emplace_back(req);
                    }
                }
            }

            for (const auto& req : requirements) {
                if (!SongCore::API::Capabilities::IsCapabilityRegistered(req)) {
                    DEBUG("Missing requirement {}", req);
                    return GlobalNamespace::EntitlementsStatus::NotOwned;
                }
            }

            return GlobalNamespace::EntitlementsStatus::Ok;
        }

        // Check beatsaver for the map
        auto beatmap = BeatSaver::API::GetBeatmapByHash(std::string(levelHash));
        if (beatmap.has_value()) {
            auto& versions = beatmap->GetVersions();
            const auto& beatmapVersion = std::find_if(versions.begin(), versions.end(), [&levelHash](const auto& v){
                auto toCheckHash = v.GetHash();
                if (toCheckHash.size() != levelHash.size()) return false;

                auto toCheckItr = toCheckHash.c_str();
                auto levelHashItr = levelHash.data();
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

            for (const auto& req : requirements) {
                if (!SongCore::API::Capabilities::IsCapabilityRegistered(req)) {
                    DEBUG("Missing requirement {}", req);
                    return GlobalNamespace::EntitlementsStatus::NotOwned;
                }
            }

            return GlobalNamespace::EntitlementsStatus::NotDownloaded;
        }

        WARNING("Level hash {} was not found locally or on beatsaver", levelHash);
        // map didn't exist anywhere, must be WIP or something...
        // either way we don't have it and can't get it
        return GlobalNamespace::EntitlementsStatus::NotOwned;
    }

    GlobalNamespace::EntitlementsStatus MpEntitlementChecker::GetKnownEntitlement(StringW userId, StringW levelId) {
        auto userDict = _entitlementsDictionary.find(userId);
        if (userDict != _entitlementsDictionary.end()) {
            auto currentEntitlement = userDict->second.find(levelId);
            if (currentEntitlement != userDict->second.end())
                return currentEntitlement->second;
        }

        return GlobalNamespace::EntitlementsStatus::Unknown;
    }

}
