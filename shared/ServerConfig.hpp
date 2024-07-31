#pragma once

#define OFFICIAL_MAX_PARTY_SIZE 5

#include "./_config.h"
#include <string>
#include "GlobalNamespace/DnsEndPoint.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

namespace MultiplayerCore {
    struct MPCORE_EXPORT ServerConfig {
        ServerConfig() {};
        ServerConfig(std::string graphUrl, std::string statusUrl, int maxPartySize = OFFICIAL_MAX_PARTY_SIZE, std::string quickPlaySetupUrl = "", bool disableSSL = true)
        : graphUrl(graphUrl), masterServerStatusUrl(statusUrl), maxPartySize(maxPartySize), quickPlaySetupUrl(quickPlaySetupUrl.empty() ? statusUrl + "/mp_override.json" : quickPlaySetupUrl), discoveryPort(0), partyPort(0), multiplayerPort(0), forceGameLift(true), disableSSL(disableSSL) { }
        ServerConfig(std::string graphUrl, std::string statusUrl, int maxPartySize = OFFICIAL_MAX_PARTY_SIZE, std::string quickPlaySetupUrl = "", int discoveryPort = 0, int partyPort = 0, int multiplayerPort = 0, bool forceGameLift = true, bool disableSSL = true)
        : graphUrl(graphUrl), masterServerStatusUrl(statusUrl), maxPartySize(maxPartySize), quickPlaySetupUrl(quickPlaySetupUrl.empty() ? statusUrl + "/mp_override.json" : quickPlaySetupUrl), discoveryPort(discoveryPort), partyPort(partyPort), multiplayerPort(multiplayerPort), forceGameLift(forceGameLift), disableSSL(disableSSL) { }

        /// @brief Serialize this to json
        /// @param allocator the allocator to use
        /// @return rapidjson value representation of this object
        rapidjson::Value toJson(rapidjson::Document::AllocatorType& allocator) const;

        /// @brief Deserialize this from json
        /// @param json the json value
        /// @return all important fields found
        bool readJson(const rapidjson::Value& json);

        std::string graphUrl;
        std::string masterServerStatusUrl;
        std::string quickPlaySetupUrl;

        int maxPartySize;
        int discoveryPort;
        int partyPort;
        int multiplayerPort;
        bool forceGameLift = true; // Deprecated
        bool disableSSL;
    };
}
