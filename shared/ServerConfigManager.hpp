#pragma once
#include <string>
#include "GlobalNamespace/DnsEndPoint.hpp"
#include "GlobalNamespace/NetworkConfigSO.hpp"
#include "Utils/event.hpp"

namespace MultiplayerCore {
    struct MasterServerConfig {
        static const int OfficialMaxPartySize = 5;
        MasterServerConfig() {}
        MasterServerConfig(std::string hostName, int port, std::string statusUrl, int maxPartySize = OfficialMaxPartySize, std::string quickPlaySetupUrl = "", int discoveryPort = 0, int partyPort = 0, int multiplayerPort = 0, bool disableGameLift = false)
            : masterServerHostName(hostName), masterServerPort(port), masterServerStatusUrl(statusUrl), maxPartySize(maxPartySize), quickPlaySetupUrl(quickPlaySetupUrl.empty() ? statusUrl + "/mp_override.json" : quickPlaySetupUrl), discoveryPort(discoveryPort), partyPort(partyPort), multiplayerPort(multiplayerPort), disableGameLift(disableGameLift) { }
                
        MasterServerConfig(GlobalNamespace::DnsEndPoint* endPoint, std::string statusUrl, int maxPartySize = OfficialMaxPartySize, std::string quickPlaySetupUrl = "", int discoveryPort = 0, int partyPort = 0, int multiplayerPort = 0, bool disableGameLift = false)
            : MasterServerConfig(endPoint->hostName, endPoint->port, statusUrl, maxPartySize, quickPlaySetupUrl, discoveryPort, partyPort, multiplayerPort, disableGameLift) { }

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

    struct ServerConfigManager {
        // Event that is fired when the master server is changed.
        static event<MasterServerConfig> MasterServerChanged;

        static void UseMasterServer(MasterServerConfig config);
        static void UseMasterServer(std::string hostName, int port, std::string statusUrl, int maxPartySize = MasterServerConfig::OfficialMaxPartySize);
        static void UseMasterServer(GlobalNamespace::DnsEndPoint* endPoint, std::string statusUrl, int maxPartySize = MasterServerConfig::OfficialMaxPartySize, std::string quickPlaySetupUrl = "");
        
        // This will set it back to the official server, which is not recommended,
        // since connecting will not work, due to missing authentication.
        static void UseOfficalServer();
        
        // Can be nullptr when called before MainSystemInit
        static GlobalNamespace::NetworkConfigSO* GetNetworkConfig();

        static MasterServerConfig GetMasterServerConfig();
    };
}