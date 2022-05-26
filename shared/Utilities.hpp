#pragma once
#include "main.hpp"
#include "songloader/shared/API.hpp"

namespace MultiplayerCore {
    struct Utilities {
        static inline std::string GetHash(const std::string& levelId) {
            return levelId.substr(RuntimeSongLoader::API::GetCustomLevelsPrefix().length(), levelId.length() - RuntimeSongLoader::API::GetCustomLevelsPrefix().length());
        }

        static inline StringW HashForLevelID(StringW levelId) {
            return levelId->StartsWith(RuntimeSongLoader::API::GetCustomLevelsPrefix().c_str()) ? levelId->Substring(RuntimeSongLoader::API::GetCustomLevelsPrefix().length()) : StringW();
        }

        static inline void ClearDelegate(Il2CppDelegate* delegate) {
            il2cpp_utils::ClearDelegate({((MethodInfo*)(void*)(delegate)->method)->methodPointer, true});
        }
    };
}