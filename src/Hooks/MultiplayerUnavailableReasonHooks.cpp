#include "hooking.hpp"
#include "logging.hpp"

#include "Models/MpStatusData.hpp"
#include "Repositories/MpStatusRepository.hpp"

#include "GlobalNamespace/MultiplayerUnavailableReason.hpp"
#include "GlobalNamespace/MultiplayerUnavailableReasonMethods.hpp"
#include "GlobalNamespace/ConnectionFailedReason.hpp"
#include "GlobalNamespace/ConnectionFailedReasonMethods.hpp"
#include "GlobalNamespace/MultiplayerPlacementErrorCodeMethods.hpp"
#include "GlobalNamespace/MultiplayerStatusData.hpp"
#include "UnityEngine/Application.hpp"

#include "scotland2/shared/loader.hpp"
#include "cpp-semver/shared/cpp-semver.hpp"
#include <memory>

static std::string requiredMod;
static std::string requiredVersion;
static std::string maximumBsVersion;

static CModResult* find_mod(std::string_view id) {
    auto mods = modloader_get_loaded();
    for (auto& modResult : std::span{mods.array, mods.size}) {
        if (modResult.info.id == id) return &modResult;
    }
    return nullptr;
};

MAKE_AUTO_HOOK_MATCH(MultiplayerUnavailableReasonMethods_TryGetMultiplayerUnavailableReason, &::GlobalNamespace::MultiplayerUnavailableReasonMethods::TryGetMultiplayerUnavailableReason, bool, GlobalNamespace::MultiplayerStatusData *data, ByRef<GlobalNamespace::MultiplayerUnavailableReason> reason) {
    reason.heldRef = GlobalNamespace::MultiplayerUnavailableReason(0);
    auto mpData = il2cpp_utils::try_cast<MultiplayerCore::Models::MpStatusData>(data).value_or(nullptr);
    if (!mpData)
    {
        if (!data) {
            DEBUG("MultiplayerStatusData is null returning MUR-1 NetworkUnreachable");
            reason.heldRef = GlobalNamespace::MultiplayerUnavailableReason::NetworkUnreachable;
            return true;
        };
        DEBUG("MultiplayerStatusData is not of type MpStatusData current code MUR-{}", reason.heldRef.value__);
        return MultiplayerUnavailableReasonMethods_TryGetMultiplayerUnavailableReason(data, reason);
    }

    auto statusRepo = MultiplayerCore::Repositories::MpStatusRepository::get_Instance();
    if (statusRepo) statusRepo->ReportStatus(mpData);
    else ERROR("StatusRepo is null");

    if (!mpData->RequiredMods.empty()) {
        for (const auto& req : mpData->RequiredMods) {
            if (!req.required) continue;
            auto installedMod = find_mod(req.id);
            if (!installedMod) continue;

            auto& installedModInfo = installedMod->info;

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
    switch(multiplayerUnavailableReason.value__) {
        case 5: { // a mod too old
            auto installedMod = find_mod(requiredMod);
            return fmt::format("Multiplayer Unavailable\nMod {} is out of date.\nPlease update to version {} or newer", installedMod->info.version, requiredVersion);
        } break;
        case 6: { // game too new
            return fmt::format("Multiplayer Unavailable\nBeat Saber version is too new\nMaximum version: {}\nCurrent version: {}", maximumBsVersion, UnityEngine::Application::get_version());
        } break;
        default:
            break;
    }
    return MultiplayerUnavailableReasonMethods_LocalizedKey(multiplayerUnavailableReason);
}


MAKE_AUTO_HOOK_MATCH(ConnectionFailedReasonMethods_LocalizedKey, &::GlobalNamespace::ConnectionFailedReasonMethods::LocalizedKey, StringW, GlobalNamespace::ConnectionFailedReason connectionFailedReason) {
    switch(connectionFailedReason.value__) {
        case 50: { // Version not compatible with lobby host
            return
                "Game Version Mismatch\n"
                "Your Beat Saber version is not compatible with the lobby hosts\n"
                "You or the host may need to either downgrade or update the game";
        }
    }
    return ConnectionFailedReasonMethods_LocalizedKey(connectionFailedReason);
}

MAKE_AUTO_HOOK_MATCH(MultiplayerPlacementErrorCodeMethods_ToConnectionFailedReason, &::GlobalNamespace::MultiplayerPlacementErrorCodeMethods::ToConnectionFailedReason, GlobalNamespace::ConnectionFailedReason, GlobalNamespace::MultiplayerPlacementErrorCode errorCode)
{
    if (errorCode.value__ >= 50)
    {
        return GlobalNamespace::ConnectionFailedReason(errorCode.value__);
    }
    return MultiplayerPlacementErrorCodeMethods_ToConnectionFailedReason(errorCode);
}