#pragma once

#include "GlobalNamespace/DnsEndPoint.hpp"
#include "GlobalNamespace/NetworkConfigSO.hpp"
#include <string>
#include <optional>

struct Hook_MainSystemInit_Init;

namespace MultiplayerCore::Hooks {
    struct NetworkConfigHooks {
        public:
            static constexpr const int OFFICIAL_MAX_PARTY_SIZE = 5;
            struct ServerConfig {
                ServerConfig() {};
                ServerConfig(std::string hostName, int port, std::string statusUrl, int maxPartySize = OFFICIAL_MAX_PARTY_SIZE, std::string quickPlaySetupUrl = "", int discoveryPort = 0, int partyPort = 0, int multiplayerPort = 0, bool disableGameLift = false)
                : masterServerHostName(hostName), masterServerPort(port), masterServerStatusUrl(statusUrl), maxPartySize(maxPartySize), quickPlaySetupUrl(quickPlaySetupUrl.empty() ? statusUrl + "/mp_override.json" : quickPlaySetupUrl), discoveryPort(discoveryPort), partyPort(partyPort), multiplayerPort(multiplayerPort), disableGameLift(disableGameLift) { }

                ServerConfig(GlobalNamespace::DnsEndPoint* endPoint, std::string statusUrl, int maxPartySize = OFFICIAL_MAX_PARTY_SIZE, std::string quickPlaySetupUrl = "", int discoveryPort = 0, int partyPort = 0, int multiplayerPort = 0, bool disableGameLift = false)
                : ServerConfig(endPoint->hostName, endPoint->port, statusUrl, maxPartySize, quickPlaySetupUrl, discoveryPort, partyPort, multiplayerPort, disableGameLift) { }

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

            static UnorderedEventCallback<const ServerConfig*> ServerChanged;
            /// @brief Use a given server config
            /// @param cfg the config to use, the pointer is not owning but you should ensure the pointer stays alive
            static void UseServerConfig(const ServerConfig* cfg);
            static void UseOfficialServer();

            static const ServerConfig* GetCurrentServerConfig();
            static const ServerConfig* GetOfficialServerConfig();

            static bool IsOverridingAPI();
        private:
            friend struct ::Hook_MainSystemInit_Init;

            static ServerConfig officialServerConfig;
            static const ServerConfig* currentServerConfig;

            static GlobalNamespace::NetworkConfigSO* networkConfig;

    };
}
