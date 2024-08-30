#pragma once

#include "./_config.h"
#include "ServerConfig.hpp"
#include "Repositories/MpStatusRepository.hpp"

namespace MultiplayerCore {
    struct MPCORE_EXPORT API {
        /// @brief revert back to the official server
        static void UseOfficialServer();

        /// @brief Switch to the provided server config
        /// @param config config to use, pointer is owned by you, so keep it alive (static memory)
        static void UseServer(const ServerConfig* config);

        /// @brief Switch to a custom server config
        /// @param graphUrl the graph url
        /// @param statusUrl the status url
        /// @param maxPartySize the max party size
        /// @param quickPlaySetupUrl the quick play setup url
        /// @param disableSSL whether to disable connecting with ssl
        static void UseCustomApiServer(const std::string& graphUrl, const std::string& statusUrl, int maxPartySize = OFFICIAL_MAX_PARTY_SIZE, const std::string& quickPlaySetupUrl = "", bool disableSSL = true);

        /// @brief Getter for the official server config
        /// @return address of the official server config
        static const ServerConfig* GetOfficialServer();

        /// @brief Getter for the currently used server config
        /// @return currently used server config
        static const ServerConfig* GetCurrentServer();

        /// @brief Event called when the server is changed
        /// @return BS-hook callback instance reference to add your event handler to
        static UnorderedEventCallback<const ServerConfig*>& GetServerChangedEvent();

        /// @brief Check whether we are overriding the api
        /// @return true for overriding, false for not
        static bool IsOverridingAPI();

        /// @brief Get the status of the last checked server
        /// @return the status data of the last checked server
        static MultiplayerCore::Models::MpStatusData* GetLastStatusData();

        /// @brief Event called when the status is updated for a given url
        /// @return BS-hook callback instance reference to add your event handler to
        static UnorderedEventCallback<std::string, const MultiplayerCore::Models::MpStatusData*>& GetStatusUpdatedForUrlEvent();
    };
}
