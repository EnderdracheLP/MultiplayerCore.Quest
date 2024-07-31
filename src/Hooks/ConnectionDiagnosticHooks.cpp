#include "hooking.hpp"
#include "logging.hpp"
#include "Utils/EnumUtils.hpp"

#include "GlobalNamespace/MultiplayerModeSelectionFlowCoordinator.hpp"
#include "GlobalNamespace/MultiplayerModeSelectionViewController.hpp"
#include "GlobalNamespace/MultiplayerLobbyConnectionController.hpp"
#include "GlobalNamespace/ConnectionFailedReason.hpp"
#include "GlobalNamespace/INetworkConfig.hpp"

// Headers for DNS resolution
#include <netdb.h>
#include <arpa/inet.h>

#include "System/Uri.hpp"


// Headers for HTTP(S) check
#include "web-utils/shared/WebUtils.hpp"
#include "web-utils/shared/Response.hpp"


MAKE_AUTO_HOOK_MATCH(MultiplayerModeSelectionFlowCoordinator_PresentConnectionErrorDialog, &GlobalNamespace::MultiplayerModeSelectionFlowCoordinator::PresentConnectionErrorDialog, void, GlobalNamespace::MultiplayerModeSelectionFlowCoordinator* self,  GlobalNamespace::MultiplayerLobbyConnectionController::LobbyConnectionType connectionType, GlobalNamespace::ConnectionFailedReason reason) {
    INFO("Connection Failed Reason: CFR-{} - {}, ConnectionType: {}", reason.value__, MultiplayerCore::Utils::EnumUtils::GetEnumName(reason), connectionType.value__);
    MultiplayerModeSelectionFlowCoordinator_PresentConnectionErrorDialog(self, connectionType, reason);

    // Diagnosing connection errors
    // Attempt dns resolve
    if (!self->_multiplayerModeSelectionViewController->_networkConfig || ! self->_multiplayerModeSelectionViewController->_networkConfig->graphUrl)
    {
        ERROR("Missing GraphUrl can't diagnose");
        return;
    }
    System::Uri* url;
    try {
        url = System::Uri::New_ctor(self->_multiplayerModeSelectionViewController->_networkConfig->graphUrl);
    }
    catch (const std::exception& e)
    {
        ERROR("Failed to parse graphUrl from config graphUrl: {}", self->_multiplayerModeSelectionViewController->_networkConfig->graphUrl);
        return;
    }
    if (!url)
    {
        ERROR("Url was null, can't proceed with diagnostics");
        return;
    }
    std::string serverAddress = static_cast<std::string>(url->Host);
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    int result = getaddrinfo(serverAddress.c_str(), nullptr, &hints, &res);
    struct sockaddr* addressToPing;
    if (result == 0) {
        // DNS resolution successful
        char ip[INET6_ADDRSTRLEN];
        void *addr;
        std::string ipStr;

        if (res->ai_family == AF_INET) { // IPv4
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)res->ai_addr;
            addr = &(ipv4->sin_addr);
            addressToPing = (struct sockaddr*)ipv4;
            inet_ntop(AF_INET, addr, ip, sizeof(ip));
            ipStr = ip;
        } else { // IPv6
            WARNING("Got an IPv6 Adress, this might not be supported by the game");
            struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)res->ai_addr;
            addr = &(ipv6->sin6_addr);
            addressToPing = (struct sockaddr*)ipv6;
            inet_ntop(AF_INET6, addr, ip, sizeof(ip));
            ipStr = ip;
        }

        INFO("DNS resolution successful. IP address: {}", ipStr);
        freeaddrinfo(res);
    } else {
        // DNS resolution failed
        INFO("DNS resolution failed: {}", gai_strerror(result));
    }

    // Attempt ping
    if (!addressToPing)
    {
        ERROR("Got no address to ping, returng!");
        return;
    }

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
    if (sock < 0) {
        ERROR("Failed to create socket for ping");
        return;
    }

    // Set timeout for receiving ping reply
    struct timeval timeout;
    timeout.tv_sec = 1; // 1 second
    timeout.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        ERROR("Failed to set socket receive timeout for ping");
        close(sock);
        return;
    }

    // Send ICMP echo request
    const int bufferSize = 32;
    char buffer[bufferSize];
    memset(buffer, 0, bufferSize);
    buffer[0] = 8; // ICMP echo request type
    buffer[1] = 0; // ICMP echo request code
    buffer[2] = 0; // ICMP echo request checksum (to be calculated)
    buffer[3] = 0; // ICMP echo request checksum (to be calculated)
    buffer[4] = 0; // ICMP echo request identifier (to be set)
    buffer[5] = 0; // ICMP echo request identifier (to be set)
    buffer[6] = 0; // ICMP echo request sequence number (to be set)
    buffer[7] = 0; // ICMP echo request sequence number (to be set)

    // Calculate ICMP echo request checksum
    unsigned short *checksumPtr = (unsigned short*)&buffer[2];
    unsigned int checksum = 0;
    for (int i = 0; i < bufferSize / 2; i++) {
        checksum += *(checksumPtr + i);
    }
    checksum = (checksum >> 16) + (checksum & 0xffff);
    checksum += (checksum >> 16);
    checksum = ~checksum;
    *checksumPtr = (unsigned short)checksum;

    // Send ICMP echo request
    if (sendto(sock, buffer, bufferSize, 0, (struct sockaddr*)&addressToPing, sizeof(addressToPing)) < 0) {
        ERROR("Failed to send ICMP echo request for ping");
        close(sock);
        return;
    }

    // Receive ICMP echo reply
    struct sockaddr_in replyAddress;
    socklen_t replyAddressLength = sizeof(replyAddress);
    if (recvfrom(sock, buffer, bufferSize, 0, (struct sockaddr*)&replyAddress, &replyAddressLength) < 0) {
        ERROR("Failed to receive ICMP echo reply for ping");
        close(sock);
        return;
    }

    // Parse ICMP echo reply
    unsigned short receivedChecksum = *(unsigned short*)&buffer[2];
    if (receivedChecksum != checksum) {
        ERROR("Received ICMP echo reply has invalid checksum");
        close(sock);
        return;
    }

    // Ping successful
    INFO("Ping successful");

    close(sock);

    // Test http(s) connection to endpoint;
    INFO("Starting HTTP(S) diagnostics");
    WebUtils::GetAsync<WebUtils::StringResponse>(WebUtils::URLOptions(static_cast<std::string>(self->_multiplayerModeSelectionViewController->_networkConfig->graphUrl)), [](WebUtils::StringResponse response) {
        if (response.get_HttpCode() == 404 || response.IsSuccessful()) {
            INFO("HTTP(S) connection successful with code: {}", response.get_HttpCode());
        } else {
            ERROR("HTTP(S) connection failed with code: {}", response.get_HttpCode());
        }

        INFO("Diagnostics Complete");
    });
    DEBUG("Diagnostics exited main hook");
}
