#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "NetworkConfigHooks.hpp"
#include "System/Security/Cryptography/X509Certificates/X509Certificate2.hpp"
#include "GlobalNamespace/DnsEndPoint.hpp"
#include "GlobalNamespace/NetworkConfigSO.hpp"
#include "GlobalNamespace/ClientCertificateValidator.hpp"
#include "GlobalNamespace/UnifiedNetworkPlayerModel.hpp"
#include "GlobalNamespace/UnifiedNetworkPlayerModel_ActiveNetworkPlayerModelType.hpp"
#include "GlobalNamespace/MainSystemInit.hpp"

namespace MultiplayerCore 
{
    GlobalNamespace::NetworkConfigSO* NetworkConfigHooks::networkConfig;
    std::string NetworkConfigHooks::masterServerHostName;
    int NetworkConfigHooks::masterServerPort;
    std::string NetworkConfigHooks::masterServerStatusUrl;
    std::string NetworkConfigHooks::quickPlaySetupUrl;
    int NetworkConfigHooks::maxPartySize;
    int NetworkConfigHooks::discoveryPort;
    int NetworkConfigHooks::partyPort;
    int NetworkConfigHooks::multiplayerPort;
    bool NetworkConfigHooks::disableGameLift;

    std::string originalMasterServerHostName;
    int originalMasterServerPort;
    std::string originalMasterServerStatusUrl;
    std::string originalQuickPlaySetupUrl;
    int originalMaxPartySize;
    bool originalForceGameLift;

    void NetworkConfigHooks::UseMasterServer(std::string hostName, int port, std::string statusUrl, int maxPartySize) {
        getLogger().debug("Master server set to '%s:%d'", hostName.c_str(), port);
        NetworkConfigHooks::masterServerHostName = hostName;
        NetworkConfigHooks::masterServerPort = port;
        NetworkConfigHooks::masterServerStatusUrl = statusUrl;
        NetworkConfigHooks::maxPartySize = maxPartySize;
        NetworkConfigHooks::quickPlaySetupUrl = statusUrl + "/mp_override.json";
        if (NetworkConfigHooks::networkConfig) {
            NetworkConfigHooks::networkConfig->masterServerHostName = NetworkConfigHooks::masterServerHostName;
            NetworkConfigHooks::networkConfig->masterServerPort = NetworkConfigHooks::masterServerPort;
            NetworkConfigHooks::networkConfig->multiplayerStatusUrl = NetworkConfigHooks::masterServerStatusUrl;
            NetworkConfigHooks::networkConfig->quickPlaySetupUrl = NetworkConfigHooks::quickPlaySetupUrl;
            NetworkConfigHooks::networkConfig->maxPartySize = NetworkConfigHooks::maxPartySize;
        }
        NetworkConfigHooks::disableGameLift = true;
    }
    void NetworkConfigHooks::UseMasterServer(GlobalNamespace::DnsEndPoint* endPoint, std::string statusUrl, int maxPartySize, std::string quickPlaySetupUrl) {
        getLogger().debug("Master server set to '%s'", static_cast<std::string>(endPoint->ToString()).c_str());
        NetworkConfigHooks::masterServerHostName = static_cast<std::string>(endPoint->hostName);
        NetworkConfigHooks::masterServerPort = endPoint->port;
        NetworkConfigHooks::masterServerStatusUrl = statusUrl;
        NetworkConfigHooks::maxPartySize = maxPartySize;
        NetworkConfigHooks::quickPlaySetupUrl = quickPlaySetupUrl.empty() ? statusUrl + "/mp_override.json" : quickPlaySetupUrl;
        if (NetworkConfigHooks::networkConfig) {
            NetworkConfigHooks::networkConfig->masterServerHostName = NetworkConfigHooks::masterServerHostName;
            NetworkConfigHooks::networkConfig->masterServerPort = NetworkConfigHooks::masterServerPort;
            NetworkConfigHooks::networkConfig->multiplayerStatusUrl = NetworkConfigHooks::masterServerStatusUrl;
            NetworkConfigHooks::networkConfig->quickPlaySetupUrl = NetworkConfigHooks::quickPlaySetupUrl;
            NetworkConfigHooks::networkConfig->maxPartySize = NetworkConfigHooks::maxPartySize;
        }
        NetworkConfigHooks::disableGameLift = true;
    }
    void NetworkConfigHooks::UseOfficalServer() {
        getLogger().debug("Master server set to 'official'");
        NetworkConfigHooks::masterServerHostName = "";
        NetworkConfigHooks::masterServerPort = 0;
        NetworkConfigHooks::masterServerStatusUrl = "";
        NetworkConfigHooks::maxPartySize = 0;
        NetworkConfigHooks::quickPlaySetupUrl = "";
        if (NetworkConfigHooks::networkConfig) {
            NetworkConfigHooks::networkConfig->masterServerHostName = originalMasterServerHostName;
            NetworkConfigHooks::networkConfig->masterServerPort = originalMasterServerPort;
            NetworkConfigHooks::networkConfig->multiplayerStatusUrl = originalMasterServerStatusUrl;
            NetworkConfigHooks::networkConfig->quickPlaySetupUrl = originalQuickPlaySetupUrl;
            NetworkConfigHooks::networkConfig->maxPartySize = originalMaxPartySize;
            NetworkConfigHooks::networkConfig->forceGameLift = originalForceGameLift;
        }
        NetworkConfigHooks::disableGameLift = false;
    }

    MAKE_HOOK_MATCH(MainSystemInit_Init, &GlobalNamespace::MainSystemInit::Init, void, GlobalNamespace::MainSystemInit* self) {
        MainSystemInit_Init(self);
        getLogger().info("Original status URL: %s", static_cast<std::string>(self->networkConfig->get_multiplayerStatusUrl()).c_str());
        getLogger().info("Original QuickPlaySetup URL: %s", static_cast<std::string>(self->networkConfig->quickPlaySetupUrl).c_str());
        // getLogger().info("ServiceEnvironment: %d", self->networkConfig->get_serviceEnvironment().value);

        originalMasterServerHostName = static_cast<std::string>(self->networkConfig->masterServerHostName);
        originalMasterServerPort = self->networkConfig->masterServerPort;
        originalMasterServerStatusUrl = static_cast<std::string>(self->networkConfig->multiplayerStatusUrl);
        originalQuickPlaySetupUrl = static_cast<std::string>(self->networkConfig->quickPlaySetupUrl);
        originalMaxPartySize = self->networkConfig->maxPartySize;
        originalForceGameLift = self->networkConfig->forceGameLift;
        NetworkConfigHooks::networkConfig = reinterpret_cast<GlobalNamespace::NetworkConfigSO*>(self->networkConfig);

        if (!NetworkConfigHooks::masterServerHostName.empty())
        {
            getLogger().debug("MainSystemInit_Init overriding MasterServerHostName to '%s'", NetworkConfigHooks::masterServerHostName.c_str());
            self->networkConfig->masterServerHostName = NetworkConfigHooks::masterServerHostName;
        }

        if (NetworkConfigHooks::masterServerPort > 0)
        {
            getLogger().debug("MainSystemInit_Init overriding MasterServerPort to '%d'", NetworkConfigHooks::masterServerPort);
            self->networkConfig->masterServerPort = NetworkConfigHooks::masterServerPort;
        }

        if (!NetworkConfigHooks::masterServerStatusUrl.empty()) {
            getLogger().debug("MainSystemInit_Init overriding MasterServerStatusUrl to '%s'", NetworkConfigHooks::masterServerStatusUrl.c_str());
            self->networkConfig->multiplayerStatusUrl = NetworkConfigHooks::masterServerStatusUrl;
        }


        if (!NetworkConfigHooks::quickPlaySetupUrl.empty()) {
            getLogger().debug("MainSystemInit_Init overriding QuickPlaySetupUrl to '%s'", NetworkConfigHooks::quickPlaySetupUrl.c_str());
            self->networkConfig->quickPlaySetupUrl = NetworkConfigHooks::quickPlaySetupUrl;
        }

        if (NetworkConfigHooks::maxPartySize > 0) {
            getLogger().debug("MainSystemInit_Init overriding MaxPartySize to '%d'", NetworkConfigHooks::maxPartySize);
            self->networkConfig->maxPartySize = NetworkConfigHooks::maxPartySize;
        }

        if (NetworkConfigHooks::disableGameLift) {
            getLogger().debug("MainSystemInit_Init overriding forceGameLift to '%s'", NetworkConfigHooks::disableGameLift ? "false" : "true");
            self->networkConfig->forceGameLift = !NetworkConfigHooks::disableGameLift;
        }
        
        getLogger().debug("MainSystemInit_Init finished");
    }

    MAKE_HOOK_MATCH(UnifiedNetworkPlayerModel_SetActiveNetworkPlayerModelType, &GlobalNamespace::UnifiedNetworkPlayerModel::SetActiveNetworkPlayerModelType, void, GlobalNamespace::UnifiedNetworkPlayerModel* self, GlobalNamespace::UnifiedNetworkPlayerModel_ActiveNetworkPlayerModelType activeNetworkPlayerModelType)
    {
        if (NetworkConfigHooks::disableGameLift)
        {
            getLogger().debug("UnifiedNetworkPlayerModel::SetActiveNetworkPlayerModelType: Disabling GameLift, Setting to MasterServer");
            return UnifiedNetworkPlayerModel_SetActiveNetworkPlayerModelType(self, GlobalNamespace::UnifiedNetworkPlayerModel_ActiveNetworkPlayerModelType::MasterServer);
        }
        getLogger().debug("UnifiedNetworkPlayerModel::SetActiveNetworkPlayerModelType: Using default");
        return UnifiedNetworkPlayerModel_SetActiveNetworkPlayerModelType(self, activeNetworkPlayerModelType);
    }

    MAKE_HOOK_MATCH(ClientCertificateValidator_ValidateCertificateChainInternal, &GlobalNamespace::ClientCertificateValidator::ValidateCertificateChainInternal, void, GlobalNamespace::ClientCertificateValidator* self, GlobalNamespace::DnsEndPoint* endPoint, System::Security::Cryptography::X509Certificates::X509Certificate2* certificate, ::ArrayW<::ArrayW<uint8_t>> certificateChain)
    {
        if (NetworkConfigHooks::masterServerHostName.empty() || NetworkConfigHooks::masterServerPort == 0)
        {
            getLogger().debug("ClientCertificateValidator::ValidateCertificateChainInternal: No EndPoint set, using default");
            return ClientCertificateValidator_ValidateCertificateChainInternal(self, endPoint, certificate, certificateChain);
        }
        getLogger().debug("ClientCertificateValidator::ValidateCertificateChainInternal: Ignoring certificate validation");
    }

    void Hooks::NetworkConfigHooks()
    {
        getLogger().debug("Installing NetworkConfig hooks...");
        INSTALL_HOOK(getLogger(), MainSystemInit_Init);
        INSTALL_HOOK(getLogger(), UnifiedNetworkPlayerModel_SetActiveNetworkPlayerModelType);
        INSTALL_HOOK(getLogger(), ClientCertificateValidator_ValidateCertificateChainInternal);
        getLogger().debug("Installed NetworkConfig hooks!");
    }
}