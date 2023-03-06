#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "NetworkConfigHooks.hpp"
#include "System/Security/Cryptography/X509Certificates/X509Certificate2.hpp"
#include "GlobalNamespace/DnsEndPoint.hpp"
#include "GlobalNamespace/NetworkConfigSO.hpp"
#include "GlobalNamespace/ClientCertificateValidator.hpp"
#include "GlobalNamespace/UnifiedNetworkPlayerModel.hpp"
#include "GlobalNamespace/UnifiedNetworkPlayerModel_ActiveNetworkPlayerModelType.hpp"

namespace MultiplayerCore 
{
    DnsEndPointW NetworkConfigHooks::masterServerEndPoint;
    std::string NetworkConfigHooks::masterServerStatusUrl;
    std::string NetworkConfigHooks::quickPlaySetupUrl;
    int NetworkConfigHooks::maxPartySize;
    int NetworkConfigHooks::discoveryPort;
    int NetworkConfigHooks::partyPort;
    int NetworkConfigHooks::multiplayerPort;
    bool NetworkConfigHooks::disableGameLift;

    void NetworkConfigHooks::UseMasterServer(DnsEndPointW endPoint, std::string statusUrl, int maxPartySize) {
        getLogger().debug("Master server set to '%s'", endPoint.ToCPPString().c_str());
        NetworkConfigHooks::masterServerEndPoint = endPoint;
        NetworkConfigHooks::masterServerStatusUrl = statusUrl;
        NetworkConfigHooks::maxPartySize = maxPartySize;
        NetworkConfigHooks::quickPlaySetupUrl = statusUrl + "/mp_override.json";
        NetworkConfigHooks::disableGameLift = true;
    }
    void NetworkConfigHooks::UseMasterServer(DnsEndPointW endPoint, std::string statusUrl, int maxPartySize, std::string quickPlaySetupUrl) {
        getLogger().debug("Master server set to '%s'", endPoint.ToCPPString().c_str());
        NetworkConfigHooks::masterServerEndPoint = endPoint;
        NetworkConfigHooks::masterServerStatusUrl = statusUrl;
        NetworkConfigHooks::maxPartySize = maxPartySize;
        NetworkConfigHooks::quickPlaySetupUrl = quickPlaySetupUrl.empty() ? statusUrl + "/mp_override.json" : quickPlaySetupUrl;
        NetworkConfigHooks::disableGameLift = true;
    }
    void NetworkConfigHooks::UseOfficalServer() {
        getLogger().debug("Master server set to 'official'");
        NetworkConfigHooks::masterServerEndPoint = DnsEndPointW();
        NetworkConfigHooks::masterServerStatusUrl = "";
        NetworkConfigHooks::maxPartySize = 0;
        NetworkConfigHooks::quickPlaySetupUrl = "";
        NetworkConfigHooks::disableGameLift = false;
    }


    MAKE_HOOK_MATCH(NetworkConfigSO_get_masterServerEndPoint, &GlobalNamespace::NetworkConfigSO::get_masterServerEndPoint, GlobalNamespace::DnsEndPoint*, GlobalNamespace::NetworkConfigSO* self)
    {
        if (!NetworkConfigHooks::masterServerEndPoint)
        {
            getLogger().debug("NetworkConfigSO::get_masterServerEndPoint: No EndPoint set, using default");
            return NetworkConfigSO_get_masterServerEndPoint(self);
        }

        getLogger().debug("NetworkConfigSO::get_masterServerEndPoint: Returning custom EndPoint");
        return NetworkConfigHooks::masterServerEndPoint;
    }

    MAKE_HOOK_MATCH(NetworkConfigSO_get_multiplayerStatusUrl, &GlobalNamespace::NetworkConfigSO::get_multiplayerStatusUrl, StringW, GlobalNamespace::NetworkConfigSO* self)
    {
        if (NetworkConfigHooks::masterServerStatusUrl.empty())
        {
            getLogger().debug("NetworkConfigSO::get_multiplayerStatusUrl: No URL set, using default");
            return NetworkConfigSO_get_multiplayerStatusUrl(self);
        }

        getLogger().debug("NetworkConfigSO::get_masterServerStatusUrl: Returning custom URL");
        return NetworkConfigHooks::masterServerStatusUrl;
    }

    MAKE_HOOK_MATCH(NetworkConfigSO_get_maxPartySize, &GlobalNamespace::NetworkConfigSO::get_maxPartySize, int, GlobalNamespace::NetworkConfigSO* self)
    {
        if (NetworkConfigHooks::maxPartySize == 0)
        {
            getLogger().debug("NetworkConfigSO::get_maxPartySize: No value set, using default");
            return NetworkConfigSO_get_maxPartySize(self);
        }

        getLogger().debug("NetworkConfigSO::get_maxPartySize: Returning custom value");
        return NetworkConfigHooks::maxPartySize;
    }

    MAKE_HOOK_MATCH(NetworkConfigSO_get_quickPlaySetupUrl, &GlobalNamespace::NetworkConfigSO::get_quickPlaySetupUrl, StringW, GlobalNamespace::NetworkConfigSO* self)
    {
        if (NetworkConfigHooks::quickPlaySetupUrl.empty())
        {
            getLogger().debug("NetworkConfigSO::get_quickPlaySetupUrl: No URL set, using default");
            return NetworkConfigSO_get_quickPlaySetupUrl(self);
        }

        getLogger().debug("NetworkConfigSO::get_quickPlaySetupUrl: Returning custom URL");
        return NetworkConfigHooks::quickPlaySetupUrl;
    }

    MAKE_HOOK_MATCH(NetworkConfigSO_get_discoveryPort, &GlobalNamespace::NetworkConfigSO::get_discoveryPort, int, GlobalNamespace::NetworkConfigSO* self)
    {
        if (NetworkConfigHooks::discoveryPort == 0)
        {
            getLogger().debug("NetworkConfigSO::get_discoveryPort: No value set, using default");
            return NetworkConfigSO_get_discoveryPort(self);
        }

        getLogger().debug("NetworkConfigSO::get_discoveryPort: Returning custom value");
        return NetworkConfigHooks::discoveryPort;
    }

    MAKE_HOOK_MATCH(NetworkConfigSO_get_partyPort, &GlobalNamespace::NetworkConfigSO::get_partyPort, int, GlobalNamespace::NetworkConfigSO* self)
    {
        if (NetworkConfigHooks::partyPort == 0)
        {
            getLogger().debug("NetworkConfigSO::get_partyPort: No value set, using default");
            return NetworkConfigSO_get_partyPort(self);
        }

        getLogger().debug("NetworkConfigSO::get_partyPort: Returning custom value");
        return NetworkConfigHooks::partyPort;
    }

    MAKE_HOOK_MATCH(NetworkConfigSO_get_multiplayerPort, &GlobalNamespace::NetworkConfigSO::get_multiplayerPort, int, GlobalNamespace::NetworkConfigSO* self)
    {
        if (NetworkConfigHooks::multiplayerPort == 0)
        {
            getLogger().debug("NetworkConfigSO::get_multiplayerPort: No value set, using default");
            return NetworkConfigSO_get_multiplayerPort(self);
        }

        getLogger().debug("NetworkConfigSO::get_multiplayerPort: Returning custom value");
        return NetworkConfigHooks::multiplayerPort;
    }

    MAKE_HOOK_MATCH(NetworkConfigSO_get_forceGameLift, &GlobalNamespace::NetworkConfigSO::get_forceGameLift, bool, GlobalNamespace::NetworkConfigSO* self)
    {
        if (NetworkConfigHooks::disableGameLift)
        {
            getLogger().debug("NetworkConfigSO::get_forceGameLift: Disabling, Returning false");
            return false;
        }

        getLogger().debug("NetworkConfigSO::get_forceGameLift: Returning default value");
        return NetworkConfigSO_get_forceGameLift(self);
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
        if (!NetworkConfigHooks::masterServerEndPoint)
        {
            getLogger().debug("ClientCertificateValidator::ValidateCertificateChainInternal: No EndPoint set, using default");
            return ClientCertificateValidator_ValidateCertificateChainInternal(self, endPoint, certificate, certificateChain);
        }
        getLogger().debug("ClientCertificateValidator::ValidateCertificateChainInternal: Ignoring certificate validation");
    }

    void Hooks::NetworkConfigHooks()
    {
        getLogger().debug("Installing NetworkConfig hooks...");
        INSTALL_HOOK(getLogger(), NetworkConfigSO_get_masterServerEndPoint);
        INSTALL_HOOK(getLogger(), NetworkConfigSO_get_multiplayerStatusUrl);
        INSTALL_HOOK(getLogger(), NetworkConfigSO_get_maxPartySize);
        INSTALL_HOOK(getLogger(), NetworkConfigSO_get_quickPlaySetupUrl);
        INSTALL_HOOK(getLogger(), NetworkConfigSO_get_discoveryPort);
        INSTALL_HOOK(getLogger(), NetworkConfigSO_get_partyPort);
        INSTALL_HOOK(getLogger(), NetworkConfigSO_get_multiplayerPort);
        INSTALL_HOOK(getLogger(), NetworkConfigSO_get_forceGameLift);
        INSTALL_HOOK(getLogger(), UnifiedNetworkPlayerModel_SetActiveNetworkPlayerModelType);
        INSTALL_HOOK(getLogger(), ClientCertificateValidator_ValidateCertificateChainInternal);
        getLogger().debug("Installed NetworkConfig hooks!");
    }
}