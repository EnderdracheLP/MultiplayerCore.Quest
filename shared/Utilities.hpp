#pragma once
#include "main.hpp"
#include "songloader/shared/API.hpp"

namespace MultiplayerCore {
    class Utilities {
    public:
        static inline std::string GetHash(const std::string& levelId) {
            return levelId.substr(RuntimeSongLoader::API::GetCustomLevelsPrefix().length(), levelId.length() - RuntimeSongLoader::API::GetCustomLevelsPrefix().length());
        }

        static inline StringW HashForLevelID(StringW levelId) {
            return levelId->StartsWith(RuntimeSongLoader::API::GetCustomLevelsPrefix().c_str()) ? levelId->Substring(RuntimeSongLoader::API::GetCustomLevelsPrefix().length()) : StringW();
            //std::string hash = static_cast<std::string>(levelId).substr(RuntimeSongLoader::API::GetCustomLevelsPrefix().length(), levelId->get_Length() - RuntimeSongLoader::API::GetCustomLevelsPrefix().length())
            //return hash.length() == 40 ? StringW(hash.c_str()) : StringW();
        }
    };
}