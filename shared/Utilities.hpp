#pragma once
#include "main.hpp"

class Utilities {
    public:
        static inline std::string GetHash(const std::string& levelId) {
            return levelId.substr(RuntimeSongLoader::API::GetCustomLevelsPrefix().length(), levelId.length() - RuntimeSongLoader::API::GetCustomLevelsPrefix().length());
        }

        // Converts a levelId to a levelHash
        static inline Il2CppString* LevelIdToHash(Il2CppString* levelId) {
            if (Il2CppString::IsNullOrWhiteSpace(levelId)) {
                return nullptr;
            }
            ArrayW<Il2CppString*> ary = levelId->Split(std::initializer_list<Il2CppChar>{ '_' }, ' ');
            Il2CppString* hash = nullptr;
            if (ary.Length() > 2) {
                hash = ary[2];
            }
            return (hash != nullptr && hash->get_Length() == 40) ? hash : nullptr;
        }

        static inline StringW HashForLevelID(StringW levelId) {
            return levelId->StartsWith(RuntimeSongLoader::API::GetCustomLevelsPrefix().c_str()) ? levelId->Substring(RuntimeSongLoader::API::GetCustomLevelsPrefix().length()) : StringW();
            //std::string hash = static_cast<std::string>(levelId).substr(RuntimeSongLoader::API::GetCustomLevelsPrefix().length(), levelId->get_Length() - RuntimeSongLoader::API::GetCustomLevelsPrefix().length())
            //return hash.length() == 40 ? StringW(hash.c_str()) : StringW();
        }
};