#include "Hooks/NetworkConfigHooks.hpp"
#include "logging.hpp"
#include "hooking.hpp"

#include "GlobalNamespace/NetworkConfigSO.hpp"
#include "GlobalNamespace/MainSystemInit.hpp"
#include "GlobalNamespace/ClientCertificateValidator.hpp"
#include "GlobalNamespace/UnifiedNetworkPlayerModel.hpp"
#include "IgnoranceCore/IgnoranceClient.hpp"
#include "System/Security/Cryptography/X509Certificates/X509Certificate2.hpp"

#define LOG_VALUE(identifier, value) DEBUG("Overriding NetworkConfigSO::" identifier " to '{}'", cfg->value)

// File is equivalent to MultiplayerCore.Patchers.NetworkConfigPatcher from PC

namespace MultiplayerCore::Hooks {
    ServerConfig NetworkConfigHooks::officialServerConfig{};
    const ServerConfig* NetworkConfigHooks::currentServerConfig{};
    GlobalNamespace::NetworkConfigSO* NetworkConfigHooks::networkConfig = nullptr;
    UnorderedEventCallback<const ServerConfig*> NetworkConfigHooks::ServerChanged{};

    const ServerConfig* NetworkConfigHooks::GetCurrentServer() { return currentServerConfig; }
    const ServerConfig* NetworkConfigHooks::GetOfficialServer() { return &officialServerConfig; }

    bool NetworkConfigHooks::IsOverridingAPI() { return GetCurrentServer() && GetCurrentServer() != GetOfficialServer(); }

    void NetworkConfigHooks::UseServer(const ServerConfig* cfg) {
        if (!cfg) {
            UseOfficialServer();
            return;
        }

        currentServerConfig = cfg;

        if (networkConfig) {
            networkConfig->_graphUrl = cfg->graphUrl;
            LOG_VALUE("graphUrl", graphUrl);
            networkConfig->_multiplayerStatusUrl = cfg->masterServerStatusUrl;
            LOG_VALUE("multiplayerStatusUrl", masterServerStatusUrl);
            networkConfig->_quickPlaySetupUrl = cfg->quickPlaySetupUrl;
            LOG_VALUE("quickPlaySetupUrl", quickPlaySetupUrl);

            // only 128 ids can exist, 0 & 127 are taken as server and broadcast
            // thus 128 - 2 = 126 absolute max player count
            networkConfig->_maxPartySize = std::clamp(cfg->maxPartySize, 0, 126);
            LOG_VALUE("maxPartySize", maxPartySize);
            networkConfig->_discoveryPort = cfg->discoveryPort;
            LOG_VALUE("discoveryPort", discoveryPort);
            networkConfig->_partyPort = cfg->partyPort;
            LOG_VALUE("partyPort", partyPort);
            networkConfig->_multiplayerPort = cfg->multiplayerPort;
            LOG_VALUE("multiplayerPort", multiplayerPort);
            networkConfig->_forceGameLift = cfg->forceGameLift;
            LOG_VALUE("forceGameLift", forceGameLift);
        }

        ServerChanged.invoke(cfg);
    }

    void NetworkConfigHooks::UseOfficialServer() {
        UseServer(&officialServerConfig);
    }
}

using namespace MultiplayerCore::Hooks;

MAKE_AUTO_HOOK_MATCH(MainSystemInit_Init, &::GlobalNamespace::MainSystemInit::Init, void, GlobalNamespace::MainSystemInit* self) {
    MainSystemInit_Init(self);
    // construct original config from base game values
    auto& officialConfig = NetworkConfigHooks::officialServerConfig;
    officialConfig.graphUrl  = static_cast<std::string>(self->_networkConfig->_graphUrl);
    officialConfig.masterServerStatusUrl  = static_cast<std::string>(self->_networkConfig->_multiplayerStatusUrl);
    officialConfig.quickPlaySetupUrl = static_cast<std::string>(self->_networkConfig->_quickPlaySetupUrl);

    officialConfig.maxPartySize = self->_networkConfig->_maxPartySize;
    officialConfig.discoveryPort = self->_networkConfig->_discoveryPort;
    officialConfig.partyPort = self->_networkConfig->_partyPort;
    officialConfig.multiplayerPort = self->_networkConfig->_multiplayerPort;
    officialConfig.forceGameLift = self->_networkConfig->_forceGameLift;

    NetworkConfigHooks::networkConfig = self->_networkConfig;

    NetworkConfigHooks::UseServer(NetworkConfigHooks::currentServerConfig);
}

MAKE_AUTO_HOOK_MATCH(UnifiedNetworkPlayerModel_SetActiveNetworkPlayerModelType, &GlobalNamespace::UnifiedNetworkPlayerModel::SetActiveNetworkPlayerModelType, void, GlobalNamespace::UnifiedNetworkPlayerModel* self, GlobalNamespace::UnifiedNetworkPlayerModel::ActiveNetworkPlayerModelType activeNetworkPlayerModelType) {
    auto currentConfig = NetworkConfigHooks::GetCurrentServer();
    if (NetworkConfigHooks::IsOverridingAPI()) {
        DEBUG("Disabling MasterServer, Setting to GameLift");
        UnifiedNetworkPlayerModel_SetActiveNetworkPlayerModelType(self, GlobalNamespace::UnifiedNetworkPlayerModel::ActiveNetworkPlayerModelType::GameLift);
        return;
    }

    DEBUG("Using Default");
    UnifiedNetworkPlayerModel_SetActiveNetworkPlayerModelType(self, activeNetworkPlayerModelType);
}

// possibly does not call orig
MAKE_AUTO_HOOK_ORIG_MATCH(ClientCertificateValidator_ValidateCertificateChainInternal, &GlobalNamespace::ClientCertificateValidator::ValidateCertificateChainInternal, void, GlobalNamespace::ClientCertificateValidator* self, GlobalNamespace::DnsEndPoint* endPoint, System::Security::Cryptography::X509Certificates::X509Certificate2* certificate, ::ArrayW<::ArrayW<uint8_t>> certificateChain) {
    if (NetworkConfigHooks::IsOverridingAPI()) {
        DEBUG("Ignoring certificate validation");
        return;
    }

    DEBUG("No EndPoint set, using default");
    ClientCertificateValidator_ValidateCertificateChainInternal(self, endPoint, certificate, certificateChain);
}

MAKE_AUTO_HOOK_MATCH(IgnoranceClient_Start, &IgnoranceCore::IgnoranceClient::Start, void, IgnoranceCore::IgnoranceClient* self) {
    if (NetworkConfigHooks::IsOverridingAPI()) {
        auto cfg = NetworkConfigHooks::GetCurrentServer();
        DEBUG("Changing IgnoranceClient ssl usage");

        self->UseSsl = !cfg->disableSSL;
        self->ValidateCertificate = !cfg->disableSSL;
    }

    IgnoranceClient_Start(self);
}
