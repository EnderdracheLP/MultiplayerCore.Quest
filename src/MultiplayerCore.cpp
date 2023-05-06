#include "MultiplayerCore.hpp"
#include "Hooks/NetworkConfigHooks.hpp"

namespace MultiplayerCore {
    void API::UseOfficialServer() {
        Hooks::NetworkConfigHooks::UseOfficialServer();
    }

    void API::UseServer(const ServerConfig* config) {
        Hooks::NetworkConfigHooks::UseServer(config);
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
