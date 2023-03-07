#pragma once
#include <string>
#include "GlobalNamespace/DnsEndPoint.hpp"
#include "GlobalNamespace/NetworkConfigSO.hpp"

namespace MultiplayerCore {
    struct NetworkConfigHooks {
        static const int OfficialMaxPartySize = 5;

        static GlobalNamespace::NetworkConfigSO* networkConfig;
        static std::string masterServerHostName;
        static int masterServerPort;
        static std::string masterServerStatusUrl;
        static std::string quickPlaySetupUrl;
        static int maxPartySize;
        static int discoveryPort;
        static int partyPort;
        static int multiplayerPort;
        static bool disableGameLift;

        static void UseMasterServer(std::string hostName, int port, std::string statusUrl, int maxPartySize = OfficialMaxPartySize);
        static void UseMasterServer(GlobalNamespace::DnsEndPoint* endPoint, std::string statusUrl, int maxPartySize = OfficialMaxPartySize, std::string quickPlaySetupUrl = "");
        static void UseOfficalServer();
    };
}