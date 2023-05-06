#pragma once

#define OFFICIAL_MAX_PARTY_SIZE 5

#include <string>
#include "GlobalNamespace/DnsEndPoint.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

namespace MultiplayerCore {
    struct ServerConfig {
        ServerConfig() {};
        ServerConfig(std::string hostName, int port, std::string statusUrl, int maxPartySize = OFFICIAL_MAX_PARTY_SIZE, std::string quickPlaySetupUrl = "", int discoveryPort = 0, int partyPort = 0, int multiplayerPort = 0, bool disableGameLift = false)
        : masterServerHostName(hostName), masterServerPort(port), masterServerStatusUrl(statusUrl), maxPartySize(maxPartySize), quickPlaySetupUrl(quickPlaySetupUrl.empty() ? statusUrl + "/mp_override.json" : quickPlaySetupUrl), discoveryPort(discoveryPort), partyPort(partyPort), multiplayerPort(multiplayerPort), disableGameLift(disableGameLift) { }

        ServerConfig(GlobalNamespace::DnsEndPoint* endPoint, std::string statusUrl, int maxPartySize = OFFICIAL_MAX_PARTY_SIZE, std::string quickPlaySetupUrl = "", int discoveryPort = 0, int partyPort = 0, int multiplayerPort = 0, bool disableGameLift = false)
        : ServerConfig(endPoint->hostName, endPoint->port, statusUrl, maxPartySize, quickPlaySetupUrl, discoveryPort, partyPort, multiplayerPort, disableGameLift) { }

        /// @brief Serialize this to json
        /// @param allocator the allocator to use
        /// @return rapidjson value representation of this object
        rapidjson::Value toJson(rapidjson::Document::AllocatorType& allocator) const;

        /// @brief Deserialize this from json
        /// @param json the json value
        /// @return all important fields found
        bool readJson(const rapidjson::Value& json);

        std::string masterServerHostName;
        int masterServerPort;
        std::string masterServerStatusUrl;
        std::string quickPlaySetupUrl;

        int maxPartySize;
        int discoveryPort;
        int partyPort;
        int multiplayerPort;
        bool disableGameLift;
    };
}
