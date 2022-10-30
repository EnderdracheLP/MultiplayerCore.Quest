#pragma once
#include "main.hpp"
#include "cpp-semver/shared/cpp-semver.hpp"
#include "logging.hpp"

namespace MultiplayerCore::Utils {
    static const std::string ChromaID = "Chroma";
    static const std::string ChromaVersionRange = ">=2.5.7";

    static bool IsInstalled(std::string const& modName) {
        auto const modList = Modloader::getMods();
        auto modInfo = modList.find(modName);
        return (modInfo != modList.end());
    }

    static bool MatchesVersion(std::string const& modName, std::string const& versionRange) {
        auto const modList = Modloader::getMods();
        // TODO: Possibly remove this
        auto modLoaded = Modloader::requireMod(modName);
        try {
            if (modLoaded) {
                auto modInfo = modList.find(modName);
                if (modInfo != modList.end()) {

                    DEBUG("Checking {} version '{}'", modName, modInfo->second.info.version);
                    if (semver::satisfies(modInfo->second.info.version, versionRange)) {
                        DEBUG("{} version valid", modName);
                        return true;
                    }
                    DEBUG("{} version outdated", modName);
                    // mod is outdated
                }
            }
            return false;
        }
        catch (const semver::semver_error &e) {
            ERROR("SemVer Exception checking {}: {}", modName, e.what());
            return false;
        }
    }
}