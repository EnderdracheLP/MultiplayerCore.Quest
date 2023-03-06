#pragma once
#include <string>
#include "Utils/DnsEndPointW.hpp"

namespace MultiplayerCore {
    struct NetworkConfigHooks {
        static const int OfficialMaxPartySize = 5;

        static DnsEndPointW masterServerEndPoint;
        static std::string masterServerStatusUrl;
        static std::string quickPlaySetupUrl;
        static int maxPartySize;
        static int discoveryPort;
        static int partyPort;
        static int multiplayerPort;
        static bool disableGameLift;

        static void UseMasterServer(DnsEndPointW endPoint, std::string statusUrl, int maxPartySize = OfficialMaxPartySize);
        static void UseMasterServer(DnsEndPointW endPoint, std::string statusUrl, int maxPartySize = OfficialMaxPartySize, std::string quickPlaySetupUrl = "");
        static void UseOfficalServer();
    };
}