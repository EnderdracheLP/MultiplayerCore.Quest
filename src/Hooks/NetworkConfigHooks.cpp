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
    ServerConfig NetworkConfigHooks::officialServerConfig = {"https://graph.oculus.com", "https://graph.oculus.com/beat_saber_multiplayer_status", OFFICIAL_MAX_PARTY_SIZE, "", false };
    const ServerConfig* NetworkConfigHooks::currentServerConfig{};
    GlobalNamespace::NetworkConfigSO* NetworkConfigHooks::networkConfig = nullptr;
    UnorderedEventCallback<const ServerConfig*> NetworkConfigHooks::ServerChanged{};

    const ServerConfig* NetworkConfigHooks::GetCurrentServer() { return currentServerConfig; }
    const ServerConfig* NetworkConfigHooks::GetOfficialServer() { return &officialServerConfig; }

    bool NetworkConfigHooks::IsOverridingAPI() { return GetCurrentServer() && GetCurrentServer() != GetOfficialServer(); }

    void NetworkConfigHooks::UseCustomApiServer(const std::string& graphUrl, const std::string& statusUrl, int maxPartySize, const std::string& quickPlaySetupUrl, bool disableSSL) {
        static ServerConfig tempConfig;
        if (graphUrl.empty() || statusUrl.empty()) {
            DEBUG("Invalid server config, using official server");
            UseOfficialServer();
            return;
        }

        tempConfig = ServerConfig(graphUrl, statusUrl, maxPartySize, quickPlaySetupUrl, disableSSL);
        currentServerConfig = &tempConfig;
        ServerChanged.invoke(currentServerConfig);
    }

    void NetworkConfigHooks::UseServer(const ServerConfig* cfg) {
        if (!cfg) {
            DEBUG("Invalid server config, using official server");
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
            networkConfig->_maxPartySize = std::clamp(cfg->maxPartySize, 2, 126);
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
        if (!networkConfig) {
            ERROR("NetworkConfigSO is null, official server not set");
            return;
        }
        UseServer(&officialServerConfig);
    }
}

using namespace MultiplayerCore::Hooks;

MAKE_AUTO_HOOK_MATCH(MainSystemInit_Init, &::GlobalNamespace::MainSystemInit::Init, void, GlobalNamespace::MainSystemInit* self, ::GlobalNamespace::SettingsApplicatorSO* settingsApplicator) {
    MainSystemInit_Init(self, settingsApplicator);
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
    auto cfg = NetworkConfigHooks::GetCurrentServer();
    DEBUG("Changing IgnoranceClient ssl usage");

    self->UseSsl = !(cfg && cfg->disableSSL);
    self->ValidateCertificate = !NetworkConfigHooks::IsOverridingAPI();

    IgnoranceClient_Start(self);
}
