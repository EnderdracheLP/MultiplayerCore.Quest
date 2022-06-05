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

        template<class T>
        requires std::is_convertible_v<T, Il2CppMulticastDelegate*>
        static inline void ClearDelegate(T delegate) {
            getLogger().debug("Clearing type: %s pointer: %p", il2cpp_utils::ClassStandardName(classof(T)).c_str(), delegate);
            il2cpp_utils::ClearDelegate({((MethodInfo*)(void*)(reinterpret_cast<Il2CppDelegate*>(delegate))->method)->methodPointer, true});
        }
    };
}