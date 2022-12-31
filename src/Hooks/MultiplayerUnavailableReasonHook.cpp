#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "Models/MpStatusData.hpp"
#include "modloader/shared/modloader.hpp"
#include "cpp-semver/shared/cpp-semver.hpp"
#include "GlobalNamespace/MultiplayerStatusData.hpp"
#include "GlobalNamespace/MultiplayerUnavailableReasonMethods.hpp"
#include "GlobalNamespace/MultiplayerUnavailableReason.hpp"
using namespace GlobalNamespace;
using namespace MultiplayerCore::Models;

namespace MultiplayerCore {

        // TODO: This only takes one mod, but it should take all mods.
        std::string _requiredMod;
        std::string _requiredVersion;

    MAKE_HOOK_MATCH(MultiplayerUnavailableReasonMethods_TryGetMultiplayerUnavailableReason, &MultiplayerUnavailableReasonMethods::TryGetMultiplayerUnavailableReason, bool, ::GlobalNamespace::MultiplayerStatusData* data, ByRef<::GlobalNamespace::MultiplayerUnavailableReason> reason)
    {
        if (data != nullptr && il2cpp_utils::AssignableFrom<Models::MpStatusData*>(reinterpret_cast<Il2CppObject*>(data)->klass) && reinterpret_cast<Models::MpStatusData*>(data)->requiredMods.size() > 0) {
            const std::unordered_map<std::string, const Mod>& installedMods = Modloader::getMods();
            for (auto& requiredMod : reinterpret_cast<Models::MpStatusData*>(data)->requiredMods) {
                auto itr = installedMods.find(requiredMod.id);
                if (itr == installedMods.end())
                    itr = installedMods.find(requiredMod.id + ".Quest");
                if (itr == installedMods.end())
                    continue;
                std::string installedVersion = itr->second.info.version;
                // We filter out our dev suffix from the installed version.
                if (installedVersion.find("-") != std::string::npos)
                    installedVersion = installedVersion.substr(0, installedVersion.find("-"));
                std::string requiredVersion = requiredMod.version;
                if (semver::valid(installedVersion) && semver::valid(requiredVersion))
                {
                    if (semver::gte(installedVersion, requiredVersion))
                        continue;
                    else 
                    {
                        getLogger().info("Installed mod %s is out of date. Required version: %s, installed version: %s", static_cast<std::string>(requiredMod.id).c_str(), requiredVersion.c_str(), installedVersion.c_str());
                        _requiredMod = static_cast<std::string>(requiredMod.id);
                        _requiredVersion = static_cast<std::string>(requiredMod.version);
                        reason = (MultiplayerUnavailableReason)5;
                        return true;
                    }
                } 
                else getLogger().error("Either installed or required mod is not semver versioned. Required version: %s, installed version: %s", requiredVersion.c_str(), installedVersion.c_str());
            }
        }
        return MultiplayerUnavailableReasonMethods_TryGetMultiplayerUnavailableReason(data, reason);
    }

    MAKE_HOOK_MATCH(MultiplayerUnavailableReasonMethods_LocalizedKey, &MultiplayerUnavailableReasonMethods::LocalizedKey, StringW, ::GlobalNamespace::MultiplayerUnavailableReason multiplayerUnavailableReason)
    {
        if (multiplayerUnavailableReason != (MultiplayerUnavailableReason)5)
            return MultiplayerUnavailableReasonMethods_LocalizedKey(multiplayerUnavailableReason);
        // var metadata = PluginManager.GetPluginFromId(_requiredMod);
        Mod metadata = Modloader::getMods().at(_requiredMod);
        return StringW(string_format("Mod %s is out of date.\nPlease update to version %s or newer.", metadata.info.id.c_str(), _requiredVersion.c_str()));
    }
    
    void Hooks::MultiplayerUnavailableReasonHook()
    {
        INSTALL_HOOK(getLogger(), MultiplayerUnavailableReasonMethods_TryGetMultiplayerUnavailableReason);
        INSTALL_HOOK(getLogger(), MultiplayerUnavailableReasonMethods_LocalizedKey);
    }
}