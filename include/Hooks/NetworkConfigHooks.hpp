#pragma once

#include "MultiplayerCore.hpp"

#include "GlobalNamespace/DnsEndPoint.hpp"
#include "GlobalNamespace/NetworkConfigSO.hpp"
#include <string>
#include <optional>

struct Hook_MainSystemInit_Init;

namespace MultiplayerCore::Hooks {
    struct NetworkConfigHooks {
        public:
            /// @brief Event for when the server changes
            static UnorderedEventCallback<const ServerConfig*> ServerChanged;

            /// @brief Use a custom API server
            /// @param graphUrl the graph url
            /// @param statusUrl the status url
            /// @param maxPartySize the max party size
            /// @param quickPlaySetupUrl the quick play setup url
            /// @param disableSSL whether to disable connecting with ssl
            static void UseCustomApiServer(const std::string& graphUrl, const std::string& statusUrl, int maxPartySize, const std::string& quickPlaySetupUrl, bool disableSSL);
            
            /// @brief Use a given server config
            /// @param cfg the config to use, the pointer is not owning but you should ensure the pointer stays alive
            static void UseServer(const ServerConfig* cfg);
            static void UseOfficialServer();

            static const ServerConfig* GetCurrentServer();
            static const ServerConfig* GetOfficialServer();

            static bool IsOverridingAPI();
        private:
            // static std::optional<std::string> GraphUrl;
            // static std::optional<std::string> MasterServerStatusUrl;
            // static std::optional<std::string> QuickPlaySetupUrl;
            // static std::optional<int> MaxPartySize;
            // static std::optional<int> DiscoveryPort;
            // static std::optional<int> PartyPort;
            // static std::optional<int> MultiplayerPort;

            // /// @brief If set: disable SSL and certificate validation for all Ignorance/ENet client connections.
            // static bool DisableSsl;

            friend struct ::Hook_MainSystemInit_Init;

            static ServerConfig officialServerConfig;
            static const ServerConfig* currentServerConfig;

            static GlobalNamespace::NetworkConfigSO* networkConfig;

            // static void UpdateNetworkConfig();

    };
    using NetworkConfigPatcher = NetworkConfigHooks;
}
