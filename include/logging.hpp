#pragma once
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include <string_view>

#include "paper/shared/logger.hpp"

namespace MultiplayerCore
{
    class Logging {
        public:
            static Logger& getLogger() {
                static Logger* logger = new Logger({MOD_ID, VERSION}, LoggerOptions(false, true));
                return *logger;
            }
    };
}

template <> struct fmt::formatter<::StringW> : formatter<string_view> {
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto format(StringW s, FormatContext& ctx) {
        return formatter<string_view>::format(static_cast<std::string>(s), ctx);
    }
};

#define INFO(...) Paper::Logger::fmtLog<Paper::LogLevel::INF>(__VA_ARGS__)
#define ERROR(...) Paper::Logger::fmtLog<Paper::LogLevel::ERR>(__VA_ARGS__)
#define CRITICAL(...) Paper::Logger::fmtLog<Paper::LogLevel::ERR>(__VA_ARGS__)
#define DEBUG(...) Paper::Logger::fmtLog<Paper::LogLevel::DBG>(__VA_ARGS__)
#define WARNING(...) Paper::Logger::fmtLog<Paper::LogLevel::WRN>(__VA_ARGS__)