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

        template<typename T>
        struct Contained
        {
            const std::vector<T>& _sequence;
            Contained(const std::vector<T> &vec) : _sequence(vec) {}
            bool operator()(T i) const 
            { 
                return _sequence.end() != std::find(_sequence.begin(), _sequence.end(), i);
            }
        };
    };
}

// For Accessing PinkCore Internal Utils that aren't exposed to the public API
namespace RequirementUtils
{
    /// @brief checks if passed requirement string is installed
	/// @param requirement the requirement to check
	/// @return bool true for installed, false for not installed
	bool GetRequirementInstalled(std::string requirement);

    /// @brief checks if passed requirement is a forced suggestion, meaning that if it is required it can be downgraded to suggestion
	/// @param requirement the requirement to check
	/// @return bool true for forced Suggestion, false for not forced suggested
	bool GetIsForcedSuggestion(std::string requirement);
}