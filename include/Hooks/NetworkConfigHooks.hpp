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
            static UnorderedEventCallback<const ServerConfig*> ServerChanged;
            /// @brief Use a given server config
            /// @param cfg the config to use, the pointer is not owning but you should ensure the pointer stays alive
            static void UseServer(const ServerConfig* cfg);
            static void UseOfficialServer();

            static const ServerConfig* GetCurrentServer();
            static const ServerConfig* GetOfficialServer();

            static bool IsOverridingAPI();
        private:
            friend struct ::Hook_MainSystemInit_Init;

            static ServerConfig officialServerConfig;
            static const ServerConfig* currentServerConfig;

            static GlobalNamespace::NetworkConfigSO* networkConfig;

    };
}
