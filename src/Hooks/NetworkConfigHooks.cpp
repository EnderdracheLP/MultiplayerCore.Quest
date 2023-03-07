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
    MasterServerConfig currentConfig;
    MasterServerConfig originalConfig;

    GlobalNamespace::NetworkConfigSO* NetworkConfigHooks::networkConfig;

    void NetworkConfigHooks::UseMasterServer(MasterServerConfig config) {
        getLogger().debug("Master server set to '%s:%d'", config.masterServerHostName.c_str(), config.masterServerPort);
        currentConfig = config;
        if (NetworkConfigHooks::networkConfig) {
            NetworkConfigHooks::networkConfig->masterServerHostName = currentConfig.masterServerHostName;
            NetworkConfigHooks::networkConfig->masterServerPort = currentConfig.masterServerPort;
            NetworkConfigHooks::networkConfig->multiplayerStatusUrl = currentConfig.masterServerStatusUrl;
            NetworkConfigHooks::networkConfig->quickPlaySetupUrl = currentConfig.quickPlaySetupUrl;
            NetworkConfigHooks::networkConfig->maxPartySize = currentConfig.maxPartySize;
            NetworkConfigHooks::networkConfig->discoveryPort = currentConfig.discoveryPort;
            NetworkConfigHooks::networkConfig->partyPort = currentConfig.partyPort;
            NetworkConfigHooks::networkConfig->multiplayerPort = currentConfig.multiplayerPort;
            NetworkConfigHooks::networkConfig->forceGameLift = currentConfig.disableGameLift;
        }
        currentConfig.disableGameLift  = true;
    }

    void NetworkConfigHooks::UseMasterServer(std::string hostName, int port, std::string statusUrl, int maxPartySize) {
        getLogger().debug("Master server set to '%s:%d'", hostName.c_str(), port);
        currentConfig.masterServerHostName = hostName;
        currentConfig.masterServerPort = port;
        currentConfig.masterServerStatusUrl = statusUrl;
        currentConfig.maxPartySize  = maxPartySize;
        currentConfig.quickPlaySetupUrl  = statusUrl + "/mp_override.json";
        currentConfig.disableGameLift  = true;
        if (NetworkConfigHooks::networkConfig) {
            NetworkConfigHooks::networkConfig->masterServerHostName = currentConfig.masterServerHostName;
            NetworkConfigHooks::networkConfig->masterServerPort = currentConfig.masterServerPort;
            NetworkConfigHooks::networkConfig->multiplayerStatusUrl = currentConfig.masterServerStatusUrl;
            NetworkConfigHooks::networkConfig->quickPlaySetupUrl = currentConfig.quickPlaySetupUrl;
            NetworkConfigHooks::networkConfig->maxPartySize = currentConfig.maxPartySize;
            NetworkConfigHooks::networkConfig->forceGameLift = currentConfig.disableGameLift;
        }
    }

    void NetworkConfigHooks::UseMasterServer(GlobalNamespace::DnsEndPoint* endPoint, std::string statusUrl, int maxPartySize, std::string quickPlaySetupUrl) {
        getLogger().debug("Master server set to '%s'", static_cast<std::string>(endPoint->ToString()).c_str());
        currentConfig.masterServerHostName  = static_cast<std::string>(endPoint->hostName);
        currentConfig.masterServerPort  = endPoint->port;
        currentConfig.masterServerStatusUrl  = statusUrl;
        currentConfig.maxPartySize  = maxPartySize;
        currentConfig.quickPlaySetupUrl  = quickPlaySetupUrl.empty() ? statusUrl + "/mp_override.json" : quickPlaySetupUrl;
        currentConfig.disableGameLift  = true;
        if (NetworkConfigHooks::networkConfig) {
            NetworkConfigHooks::networkConfig->masterServerHostName = currentConfig.masterServerHostName;
            NetworkConfigHooks::networkConfig->masterServerPort = currentConfig.masterServerPort;
            NetworkConfigHooks::networkConfig->multiplayerStatusUrl = currentConfig.masterServerStatusUrl;
            NetworkConfigHooks::networkConfig->quickPlaySetupUrl = currentConfig.quickPlaySetupUrl;
            NetworkConfigHooks::networkConfig->maxPartySize = currentConfig.maxPartySize;
            NetworkConfigHooks::networkConfig->forceGameLift = currentConfig.disableGameLift;
        }
    }
    void NetworkConfigHooks::UseOfficalServer() {
        getLogger().debug("Master server set to 'official'");
        currentConfig.masterServerHostName  = "";
        currentConfig.masterServerPort  = 0;
        currentConfig.masterServerStatusUrl  = "";
        currentConfig.maxPartySize  = 0;
        currentConfig.quickPlaySetupUrl  = "";
        currentConfig.disableGameLift  = false;
        if (NetworkConfigHooks::networkConfig) {
            NetworkConfigHooks::networkConfig->masterServerHostName = originalConfig.masterServerHostName;
            NetworkConfigHooks::networkConfig->masterServerPort = originalConfig.masterServerPort;
            NetworkConfigHooks::networkConfig->multiplayerStatusUrl = originalConfig.masterServerStatusUrl;
            NetworkConfigHooks::networkConfig->quickPlaySetupUrl = originalConfig.quickPlaySetupUrl;
            NetworkConfigHooks::networkConfig->maxPartySize = originalConfig.maxPartySize;
            NetworkConfigHooks::networkConfig->forceGameLift = originalConfig.disableGameLift;
            NetworkConfigHooks::networkConfig->discoveryPort = originalConfig.discoveryPort;
            NetworkConfigHooks::networkConfig->partyPort = originalConfig.partyPort;
            NetworkConfigHooks::networkConfig->multiplayerPort = originalConfig.multiplayerPort;
        }
    }

    MAKE_HOOK_MATCH(MainSystemInit_Init, &GlobalNamespace::MainSystemInit::Init, void, GlobalNamespace::MainSystemInit* self) {
        MainSystemInit_Init(self);
        getLogger().info("Original status URL: %s", static_cast<std::string>(self->networkConfig->get_multiplayerStatusUrl()).c_str());
        getLogger().info("Original QuickPlaySetup URL: %s", static_cast<std::string>(self->networkConfig->quickPlaySetupUrl).c_str());
        // getLogger().info("ServiceEnvironment: %d", self->networkConfig->get_serviceEnvironment().value);

        originalConfig.masterServerHostName  = static_cast<std::string>(self->networkConfig->masterServerHostName);
        originalConfig.masterServerPort  = self->networkConfig->masterServerPort;
        originalConfig.masterServerStatusUrl  = static_cast<std::string>(self->networkConfig->multiplayerStatusUrl);
        originalConfig.quickPlaySetupUrl = static_cast<std::string>(self->networkConfig->quickPlaySetupUrl);
        originalConfig.maxPartySize = self->networkConfig->maxPartySize;
        originalConfig.disableGameLift = self->networkConfig->forceGameLift;
        originalConfig.discoveryPort = self->networkConfig->discoveryPort;
        originalConfig.partyPort = self->networkConfig->partyPort;
        originalConfig.multiplayerPort = self->networkConfig->multiplayerPort;
        NetworkConfigHooks::networkConfig = reinterpret_cast<GlobalNamespace::NetworkConfigSO*>(self->networkConfig);

        if (!currentConfig.masterServerHostName .empty())
        {
            getLogger().debug("MainSystemInit_Init overriding MasterServerHostName to '%s'", currentConfig.masterServerHostName .c_str());
            self->networkConfig->masterServerHostName = currentConfig.masterServerHostName ;
        }

        if (currentConfig.masterServerPort  > 0)
        {
            getLogger().debug("MainSystemInit_Init overriding MasterServerPort to '%d'", currentConfig.masterServerPort );
            self->networkConfig->masterServerPort = currentConfig.masterServerPort ;
        }

        if (!currentConfig.masterServerStatusUrl .empty()) {
            getLogger().debug("MainSystemInit_Init overriding MasterServerStatusUrl to '%s'", currentConfig.masterServerStatusUrl .c_str());
            self->networkConfig->multiplayerStatusUrl = currentConfig.masterServerStatusUrl ;
        }


        if (!currentConfig.quickPlaySetupUrl .empty()) {
            getLogger().debug("MainSystemInit_Init overriding QuickPlaySetupUrl to '%s'", currentConfig.quickPlaySetupUrl .c_str());
            self->networkConfig->quickPlaySetupUrl = currentConfig.quickPlaySetupUrl ;
        }

        if (currentConfig.maxPartySize  > 0) {
            getLogger().debug("MainSystemInit_Init overriding MaxPartySize to '%d'", currentConfig.maxPartySize );
            self->networkConfig->maxPartySize = currentConfig.maxPartySize ;
        }

        if (currentConfig.disableGameLift ) {
            getLogger().debug("MainSystemInit_Init overriding forceGameLift to '%s'", currentConfig.disableGameLift  ? "false" : "true");
            self->networkConfig->forceGameLift = !currentConfig.disableGameLift ;
        }
        
        getLogger().debug("MainSystemInit_Init finished");
    }

    MAKE_HOOK_MATCH(UnifiedNetworkPlayerModel_SetActiveNetworkPlayerModelType, &GlobalNamespace::UnifiedNetworkPlayerModel::SetActiveNetworkPlayerModelType, void, GlobalNamespace::UnifiedNetworkPlayerModel* self, GlobalNamespace::UnifiedNetworkPlayerModel_ActiveNetworkPlayerModelType activeNetworkPlayerModelType)
    {
        if (currentConfig.disableGameLift )
        {
            getLogger().debug("UnifiedNetworkPlayerModel::SetActiveNetworkPlayerModelType: Disabling GameLift, Setting to MasterServer");
            return UnifiedNetworkPlayerModel_SetActiveNetworkPlayerModelType(self, GlobalNamespace::UnifiedNetworkPlayerModel_ActiveNetworkPlayerModelType::MasterServer);
        }
        getLogger().debug("UnifiedNetworkPlayerModel::SetActiveNetworkPlayerModelType: Using default");
        return UnifiedNetworkPlayerModel_SetActiveNetworkPlayerModelType(self, activeNetworkPlayerModelType);
    }

    MAKE_HOOK_MATCH(ClientCertificateValidator_ValidateCertificateChainInternal, &GlobalNamespace::ClientCertificateValidator::ValidateCertificateChainInternal, void, GlobalNamespace::ClientCertificateValidator* self, GlobalNamespace::DnsEndPoint* endPoint, System::Security::Cryptography::X509Certificates::X509Certificate2* certificate, ::ArrayW<::ArrayW<uint8_t>> certificateChain)
    {
        if (currentConfig.masterServerHostName .empty() || currentConfig.masterServerPort  == 0)
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