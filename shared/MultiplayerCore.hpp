#pragma once

#include "./_config.h"
#include "ServerConfig.hpp"

namespace MultiplayerCore {
    struct MPCORE_EXPORT API {
        /// @brief revert back to the official server, but since there's no auth on quest might be useless
        static void UseOfficialServer();

        /// @brief Switch to the provided server config
        /// @param config config to use, pointer is owned by you, so keep it alive (static memory)
        static void UseServer(const ServerConfig* config);

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
    };
}
