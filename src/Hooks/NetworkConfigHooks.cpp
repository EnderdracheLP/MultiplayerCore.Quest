#include "Hooks/NetworkConfigHooks.hpp"
#include "logging.hpp"
#include "hooking.hpp"

#include "GlobalNamespace/NetworkConfigSO.hpp"
#include "GlobalNamespace/MainSystemInit.hpp"
#include "GlobalNamespace/ClientCertificateValidator.hpp"
#include "GlobalNamespace/UnifiedNetworkPlayerModel.hpp"
#include "GlobalNamespace/UnifiedNetworkPlayerModel_ActiveNetworkPlayerModelType.hpp"
#include "System/Security/Cryptography/X509Certificates/X509Certificate2.hpp"

#define LOG_VALUE(identifier, value) DEBUG("Overriding NetworkConfigSO::" identifier " to '{}'", cfg->value)

// File is equivalent to MultiplayerCore.Patchers.NetworkConfigPatcher from PC

namespace MultiplayerCore::Hooks {
    NetworkConfigHooks::ServerConfig NetworkConfigHooks::officialServerConfig{};
    const NetworkConfigHooks::ServerConfig* NetworkConfigHooks::currentServerConfig{};
    GlobalNamespace::NetworkConfigSO* NetworkConfigHooks::networkConfig = nullptr;
    UnorderedEventCallback<const NetworkConfigHooks::ServerConfig*> NetworkConfigHooks::ServerChanged{};

    const NetworkConfigHooks::ServerConfig* NetworkConfigHooks::GetCurrentServerConfig() { return currentServerConfig; }
    const NetworkConfigHooks::ServerConfig* NetworkConfigHooks::GetOfficialServerConfig() { return &officialServerConfig; }

    bool NetworkConfigHooks::IsOverridingAPI() { return GetCurrentServerConfig() != GetOfficialServerConfig(); }

    void NetworkConfigHooks::UseServerConfig(const ServerConfig* cfg) {
        if (!cfg) {
            UseOfficialServer();
            return;
        }

        currentServerConfig = cfg;

        if (networkConfig) {
            networkConfig->masterServerHostName = cfg->masterServerHostName;
            LOG_VALUE("masterServerHostName", masterServerHostName);
            networkConfig->masterServerPort = cfg->masterServerPort;
            LOG_VALUE("masterServerPort", masterServerPort);
            networkConfig->multiplayerStatusUrl = cfg->masterServerStatusUrl;
            LOG_VALUE("multiplayerStatusUrl", masterServerStatusUrl);
            networkConfig->quickPlaySetupUrl = cfg->quickPlaySetupUrl;
            LOG_VALUE("quickPlaySetupUrl", quickPlaySetupUrl);
            networkConfig->maxPartySize = cfg->maxPartySize;
            LOG_VALUE("maxPartySize", maxPartySize);
            networkConfig->forceGameLift = cfg->disableGameLift;
            LOG_VALUE("forceGameLift", disableGameLift);
            networkConfig->discoveryPort = cfg->discoveryPort;
            LOG_VALUE("discoveryPort", discoveryPort);
            networkConfig->partyPort = cfg->partyPort;
            LOG_VALUE("partyPort", partyPort);
            networkConfig->multiplayerPort = cfg->multiplayerPort;
            LOG_VALUE("multiplayerPort", multiplayerPort);
        }

        ServerChanged.invoke(cfg);
    }

    void NetworkConfigHooks::UseOfficialServer() {
        UseServerConfig(&officialServerConfig);
    }
}

using namespace MultiplayerCore::Hooks;

MAKE_AUTO_HOOK_MATCH(MainSystemInit_Init, &::GlobalNamespace::MainSystemInit::Init, void, GlobalNamespace::MainSystemInit* self) {
    MainSystemInit_Init(self);
    // construct original config from base game values
    auto& officialConfig = NetworkConfigHooks::officialServerConfig;
    officialConfig.masterServerHostName  = static_cast<std::string>(self->networkConfig->masterServerHostName);
    officialConfig.masterServerPort  = self->networkConfig->masterServerPort;
    officialConfig.masterServerStatusUrl  = static_cast<std::string>(self->networkConfig->multiplayerStatusUrl);
    officialConfig.quickPlaySetupUrl = static_cast<std::string>(self->networkConfig->quickPlaySetupUrl);
    officialConfig.maxPartySize = self->networkConfig->maxPartySize;
    officialConfig.disableGameLift = self->networkConfig->forceGameLift;
    officialConfig.discoveryPort = self->networkConfig->discoveryPort;
    officialConfig.partyPort = self->networkConfig->partyPort;
    officialConfig.multiplayerPort = self->networkConfig->multiplayerPort;
    officialConfig.disableGameLift = false;
    NetworkConfigHooks::networkConfig = self->networkConfig;

    NetworkConfigHooks::UseServerConfig(NetworkConfigHooks::currentServerConfig);
}

MAKE_AUTO_HOOK_MATCH(UnifiedNetworkPlayerModel_SetActiveNetworkPlayerModelType, &GlobalNamespace::UnifiedNetworkPlayerModel::SetActiveNetworkPlayerModelType, void, GlobalNamespace::UnifiedNetworkPlayerModel* self, GlobalNamespace::UnifiedNetworkPlayerModel_ActiveNetworkPlayerModelType activeNetworkPlayerModelType) {
    auto currentConfig = NetworkConfigHooks::GetCurrentServerConfig();
    if (currentConfig && currentConfig->disableGameLift) {
        DEBUG("Disabling GameLift, Setting to MasterServer");
        UnifiedNetworkPlayerModel_SetActiveNetworkPlayerModelType(self, GlobalNamespace::UnifiedNetworkPlayerModel_ActiveNetworkPlayerModelType::MasterServer);
        return;
    }
    DEBUG("Using Default");
    UnifiedNetworkPlayerModel_SetActiveNetworkPlayerModelType(self, activeNetworkPlayerModelType);
}

MAKE_AUTO_HOOK_ORIG_MATCH(ClientCertificateValidator_ValidateCertificateChainInternal, &GlobalNamespace::ClientCertificateValidator::ValidateCertificateChainInternal, void, GlobalNamespace::ClientCertificateValidator* self, GlobalNamespace::DnsEndPoint* endPoint, System::Security::Cryptography::X509Certificates::X509Certificate2* certificate, ::ArrayW<::ArrayW<uint8_t>> certificateChain) {
    auto currentConfig = NetworkConfigHooks::GetCurrentServerConfig();
    if (!currentConfig || currentConfig->masterServerHostName.empty() || currentConfig->masterServerPort == 0) {
        DEBUG("No EndPoint set, using default");
        ClientCertificateValidator_ValidateCertificateChainInternal(self, endPoint, certificate, certificateChain);
        return;
    }
    DEBUG("Ignoring certificate validation");
}
