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

namespace MultiplayerCore::Hooks {

    unsigned short calculateChecksum(unsigned short *addr, int len) {
        int nleft = len;
        unsigned short *w = addr;
        unsigned short answer;
        int sum = 0;

        while (nleft > 1) {
            sum += *w++;
            nleft -= sizeof(unsigned short);
        }

        if (nleft == 1) {
            unsigned short u = 0;
            *(unsigned char*)(&u) = *(unsigned char*)w;
            sum += u;
        }

        sum = (sum >> 16) + (sum & 0xffff);
        sum += (sum >> 16);
        answer = ~sum;
        return answer;
    }

    void Ping(struct sockaddr_in addressToPing) {
        // if (!addressToPing) {
        //     ERROR("Got no address to ping");
        //     return;
        // }

        // Ensure the address size is set for IPv4
        // socklen_t addressSize = sizeof(struct sockaddr_in);

        // Debugging information
        if (addressToPing.sin_family != AF_INET) {
            ERROR("Unexpected address family: {}", addressToPing.sin_family);
            return;
        }

        DEBUG("Starting ping diagnostics");

        // int sock = socket(addressToPing->sa_family, SOCK_DGRAM, addressToPing->sa_family == AF_INET ? IPPROTO_ICMP : IPPROTO_ICMPV6);
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
        unsigned short sentChecksum = calculateChecksum((unsigned short*)buffer, bufferSize);

        // Set the checksum in the buffer
        ((unsigned short*)buffer)[1] = sentChecksum;

        DEBUG("Checksum of ICMP echo request: {}", sentChecksum);

        // Send ICMP echo request
        if (sendto(sock, buffer, bufferSize, 0, (struct sockaddr*)&addressToPing, sizeof(struct sockaddr_in)) < 0) {
            ERROR("Failed to send ICMP echo request for ping: {}", strerror(errno));
            close(sock);
            return;
        }

        // Receive ICMP echo reply
        struct sockaddr_storage replyAddress;
        socklen_t replyAddressLength = sizeof(replyAddress);
        if (recvfrom(sock, buffer, bufferSize, 0, (struct sockaddr*)&replyAddress, &replyAddressLength) < 0) {
            ERROR("Failed to receive ICMP echo reply for ping: {}", strerror(errno));
            close(sock);
            return;
        }

        // Check if reply is of correct type
        if (buffer[0] != 0) {
            ERROR("Received ICMP echo reply is not of type 0, type: {}", buffer[0]);
            // Print name of ICMP type using possible replies for an ICMP echo request
            switch (buffer[0]) {
                case 3:
                    ERROR("ICMP Type 3: Destination Unreachable");
                    break;
                case 4:
                    ERROR("ICMP Type 4: Source Quench");
                    break;
                case 5:
                    ERROR("ICMP Type 5: Redirect");
                    break;
                case 11:
                    ERROR("ICMP Type 11: Time Exceeded");
                    break;
                case 12:
                    ERROR("ICMP Type 12: Parameter Problem");
                    break;
                default:
                    ERROR("Unknown ICMP Type");
                    break;
            }
            close(sock);
            return;
        }

        // Compare the recalculated checksum with the extracted checksum
        // TODO: Figure out why the checksum is not correct
        unsigned short receivedChecksum = *(unsigned short*)&buffer[2];
        *(unsigned short*)&buffer[2] = 0;
        unsigned short recalculatedChecksum = calculateChecksum((unsigned short*)buffer, bufferSize);
        if (recalculatedChecksum != receivedChecksum) {
            ERROR("Received ICMP echo reply has invalid checksum");
            ERROR("Expected checksum: {}", recalculatedChecksum);
            ERROR("Received checksum: {}", receivedChecksum);
            close(sock);
            return;
        }
        // Ping successful
        INFO("Ping successful");

        close(sock);

        INFO("Finished ping diagnostics");
    }
}


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

    INFO("Starting DNS diagnostics");
    std::string serverAddress = static_cast<std::string>(url->Host);
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    // hints.ai_family = AF_UNSPEC;
    hints.ai_family = AF_INET; // For now we only want the IPv4 address
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    int result = getaddrinfo(serverAddress.c_str(), nullptr, &hints, &res);
    struct sockaddr_in addressToPing/* = nullptr*/;
    if (result == 0) {
        // DNS resolution successful
        char ip[INET6_ADDRSTRLEN];
        void *addr;
        std::string ipStr;

        if (res->ai_family == AF_INET) { // IPv4
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)res->ai_addr;
            addr = &(ipv4->sin_addr);
            // addressToPing = (struct sockaddr*)ipv4;
            // addressToPing = ipv4;
            // Allocate memory for addressToPing and copy the contents
            // addressToPing = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
            // if (addressToPing == nullptr) {
            //     ERROR("Failed to allocate memory for addressToPing");
            //     freeaddrinfo(res);
            //     return;
            // }
            memcpy(&addressToPing, ipv4, sizeof(struct sockaddr_in));
            
            inet_ntop(AF_INET, addr, ip, sizeof(ip));
            ipStr = ip;
        } else { // IPv6
            WARNING("Got an IPv6 Adress, this might not be supported by the game");
            struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)res->ai_addr;
            addr = &(ipv6->sin6_addr);
            // addressToPing = (struct sockaddr*)ipv6;
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
    // if (!addressToPing)
    // {
    //     ERROR("Got no address to ping or address was IPv6!");
    // }
    // else {
    //     // Create a separate ping thread
        DEBUG("Starting ping thread");
        std::thread(MultiplayerCore::Hooks::Ping, addressToPing).detach();
    // }

    // Test http(s) connection to endpoint;
    INFO("Starting HTTP(S) diagnostics, request to {}", self->_multiplayerModeSelectionViewController->_networkConfig->graphUrl);
    WebUtils::URLOptions urlOptions = WebUtils::URLOptions(static_cast<std::string>(self->_multiplayerModeSelectionViewController->_networkConfig->graphUrl));
    urlOptions.noEscape = true;
    WebUtils::GetAsync<WebUtils::StringResponse>(urlOptions, [](WebUtils::StringResponse response) {
        if (response.get_HttpCode() == 404 || response.IsSuccessful()) {
            INFO("HTTP(S) connection successful with code: {} curlStatus: {}", response.get_HttpCode(), response.get_CurlStatus());
        } else {
            ERROR("HTTP(S) connection failed with code: {} curlStatus: {}", response.get_HttpCode(), response.get_CurlStatus());
        }

        INFO("HTTP(S) Diagnostics Complete");
    });
    DEBUG("Diagnostics exited main hook");
}
