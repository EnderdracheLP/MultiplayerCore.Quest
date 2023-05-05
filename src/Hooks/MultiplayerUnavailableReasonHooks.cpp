#include "hooking.hpp"
#include "logging.hpp"

#include "Models/MpStatusData.hpp"

#include "GlobalNamespace/MultiplayerUnavailableReason.hpp"
#include "GlobalNamespace/MultiplayerUnavailableReasonMethods.hpp"
#include "GlobalNamespace/MultiplayerStatusData.hpp"
#include "UnityEngine/Application.hpp"

#include "modloader/shared/modloader.hpp"
#include "cpp-semver/shared/cpp-semver.hpp"

static std::string requiredMod;
static std::string requiredVersion;
static std::string maximumBsVersion;

MAKE_AUTO_HOOK_MATCH(MultiplayerUnavailableReasonMethods_TryGetMultiplayerUnavailableReason, &::GlobalNamespace::MultiplayerUnavailableReasonMethods::TryGetMultiplayerUnavailableReason, bool, GlobalNamespace::MultiplayerStatusData *data, ByRef<GlobalNamespace::MultiplayerUnavailableReason> reason) {
    reason.heldRef = GlobalNamespace::MultiplayerUnavailableReason(0);
    auto mpData = il2cpp_utils::try_cast<MultiplayerCore::Models::MpStatusData>(data).value_or(nullptr);
    if (!mpData) return MultiplayerUnavailableReasonMethods_TryGetMultiplayerUnavailableReason(data, reason);

    if (!mpData->requiredMods.empty()) {
        auto mods = Modloader::getMods();
        for (const auto& req : mpData->requiredMods) {
            // TODO: check if this is how to find it
            auto installedMod = mods.find(req.id);
            if (installedMod == mods.end()) continue;
            auto& installedModInfo = installedMod->second.info;

            auto& requiredVersion = req.version;
            auto& installedVersion = installedModInfo.version;
            if (semver::lte(requiredVersion, installedVersion)) continue;

            reason.heldRef = GlobalNamespace::MultiplayerUnavailableReason(5);
            ::requiredMod = installedModInfo.id;
            ::requiredVersion = installedModInfo.version;

            return true;
        }
    }

    if (mpData->maximumAppVersion) {
        auto gameVersion = UnityEngine::Application::get_version();
        if (semver::gt(mpData->maximumAppVersion, gameVersion)) {
            reason.heldRef = GlobalNamespace::MultiplayerUnavailableReason(6);
            maximumBsVersion = static_cast<std::string>(mpData->maximumAppVersion);
            return true;
        }
    }

    return MultiplayerUnavailableReasonMethods_TryGetMultiplayerUnavailableReason(data, reason);
}

MAKE_AUTO_HOOK_MATCH(MultiplayerUnavailableReasonMethods_LocalizedKey, &::GlobalNamespace::MultiplayerUnavailableReasonMethods::LocalizedKey, StringW, GlobalNamespace::MultiplayerUnavailableReason multiplayerUnavailableReason) {
    switch(multiplayerUnavailableReason.value) {
        case 5: { // a mod too old
            auto mods = Modloader::getMods();
            auto installedMod = mods.find(requiredMod);
            return fmt::format("Multiplayer Unavailable\nMod {} is out of date.\nPlease update to version {} or newer", installedMod->second.name, requiredVersion);
        } break;
        case 6: { // game too old
            return fmt::format("Multiplayer Unavailable\nBeat Saber version is too new\nMaximum version: {}\nCurrent version: {}", maximumBsVersion, UnityEngine::Application::get_version());
        } break;
        default:
            break;
    }
    return MultiplayerUnavailableReasonMethods_LocalizedKey(multiplayerUnavailableReason);
}
