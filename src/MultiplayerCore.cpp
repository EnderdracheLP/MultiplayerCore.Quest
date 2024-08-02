#include "MultiplayerCore.hpp"
#include "Hooks/NetworkConfigHooks.hpp"

namespace MultiplayerCore {
    void API::UseOfficialServer() {
        Hooks::NetworkConfigHooks::UseOfficialServer();
    }

    void API::UseServer(const ServerConfig* config) {
        Hooks::NetworkConfigHooks::UseServer(config);
    }

    void API::UseCustomApiServer(const std::string& graphUrl, const std::string& statusUrl, int maxPartySize, const std::string& quickPlaySetupUrl, bool disableSSL) {
        Hooks::NetworkConfigHooks::UseCustomApiServer(graphUrl, statusUrl, maxPartySize, quickPlaySetupUrl, disableSSL);
    }

    const ServerConfig* API::GetOfficialServer() {
        return Hooks::NetworkConfigHooks::GetOfficialServer();
    }

    const ServerConfig* API::GetCurrentServer() {
        return Hooks::NetworkConfigHooks::GetCurrentServer();
    }

    UnorderedEventCallback<const ServerConfig*>& API::GetServerChangedEvent() {
        return Hooks::NetworkConfigHooks::ServerChanged;
    }

    bool API::IsOverridingAPI() {
        return Hooks::NetworkConfigHooks::IsOverridingAPI();
    }
}
