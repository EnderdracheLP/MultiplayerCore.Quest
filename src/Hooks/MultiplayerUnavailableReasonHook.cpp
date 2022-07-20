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
        if (il2cpp_utils::AssignableFrom<Models::MpStatusData*>(reinterpret_cast<Il2CppObject*>(data)->klass) && reinterpret_cast<Models::MpStatusData*>(data)->requiredMods.size() > 0) {
            const std::unordered_map<std::string, const Mod>& installedMods = Modloader::getMods();
            for (auto& requiredMod : reinterpret_cast<Models::MpStatusData*>(data)->requiredMods) {
                if (installedMods.find(requiredMod.id) == installedMods.end())
                    continue;
                else {
                    std::string installedVersion = installedMods.at(requiredMod.id).info.version;
                    // We filter out our dev suffix from the installed version.
                    if (installedVersion.find("-") != std::string::npos)
                        installedVersion = installedVersion.substr(0, installedVersion.find("-"));
                    std::string requiredVersion = requiredMod.version;
                    if (semver::valid(installedVersion) && semver::valid(requiredVersion)) {
                        if (semver::gte(installedVersion, requiredVersion))
                            continue;
                        // else if (semver::major(installedVersion) >= semver::major(requiredVersion)
                        //     && semver::minor(installedVersion) >= semver::minor(requiredVersion)) {
                        //     continue;
                        // }
                        else {
                            getLogger().info("Installed mod %s is out of date. Required version: %s, installed version: %s", static_cast<std::string>(requiredMod.id).c_str(), requiredVersion.c_str(), installedVersion.c_str());
                            _requiredMod = static_cast<std::string>(requiredMod.id);
                            _requiredVersion = static_cast<std::string>(requiredMod.version);
                            reason = (MultiplayerUnavailableReason)5;
                            return true;
                        }
                    } else 
                        getLogger().error("Either installed or required mod is not semver versioned. Required version: %s, installed version: %s", requiredVersion.c_str(), installedVersion.c_str());
                    // const std::string requiredVersion = requiredMod.version;
                    // const std::string& installedVersion = installedMods.at(requiredMod.id).info.version;
                    // // Splits version numbers into a string vector and checks major and minor version numbers ignoring patch version
                    // std::vector<std::string> requiredVersionSplit = il2cpp_utils::split(requiredVersion, '.');
                    // std::vector<std::string> installedVersionSplit = il2cpp_utils::split(installedVersion, '.');
                    // int numbersToCompare = std::min(std::min(requiredVersionSplit.size(), installedVersionSplit.size()), 2);
                    // for (int i = 0; i < numbersToCompare; i++) {
                    //     if (std::stoi(requiredVersionSplit.at(i)) > std::stoi(installedVersionSplit.at(i)))
                    //     {
                    //         _requiredMod = requiredMod.id;
                    //         _requiredVersion = requiredVersion;
                    //         reason = (MultiplayerUnavailableReason)5;
                    //         return true;
                    //     }
                    //     else if (std::stoi(requiredVersionSplit.at(i)) <= std::stoi(installedVersionSplit.at(i)))
                    //         continue;
                    // }
                }
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