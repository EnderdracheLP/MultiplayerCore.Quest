#include "ServerConfig.hpp"

#define ADD_MEMBER(name) value.AddMember(#name, name, allocator);
#define ADD_STRING(str) value.AddMember(#str, rapidjson::Value(str.c_str(), str.length(), allocator), allocator)

#define GET_STRING(str, important) \
    auto str##Itr = json.FindMember(#str); \
    if (str##Itr != json.MemberEnd() && str##Itr->value.IsString()) { \
        str = std::string(str##Itr->value.GetString(), str##Itr->value.GetStringLength());\
    } else if constexpr (important) {\
        missedAnyImportantFields = true;\
    }

#define GET_T(identifier, type, important)  \
    auto identifier##Itr = json.FindMember(#identifier); \
    if (identifier##Itr != json.MemberEnd() && identifier##Itr->value.Is##type()) { \
        identifier = identifier##Itr->value.Get##type(); \
    } else if constexpr (important) {\
        missedAnyImportantFields = true;\
    }

namespace MultiplayerCore {
    rapidjson::Value ServerConfig::toJson(rapidjson::Document::AllocatorType& allocator) const {
        rapidjson::Value value;
        value.SetObject();

        ADD_STRING(graphUrl);
        ADD_STRING(masterServerStatusUrl);
        ADD_STRING(quickPlaySetupUrl);

        ADD_MEMBER(maxPartySize);
        ADD_MEMBER(discoveryPort);
        ADD_MEMBER(partyPort);
        ADD_MEMBER(multiplayerPort);
        ADD_MEMBER(forceGameLift);

        return value;
    }

    bool ServerConfig::readJson(const rapidjson::Value& json) {
        bool missedAnyImportantFields = false;

        GET_STRING(graphUrl, true);
        GET_STRING(masterServerStatusUrl, true);
        GET_STRING(quickPlaySetupUrl, false);

        GET_T(maxPartySize, Int, false);
        GET_T(discoveryPort, Int, false);
        GET_T(partyPort, Int, false);
        GET_T(multiplayerPort, Int, false);
        GET_T(forceGameLift, Bool, true);

        return !missedAnyImportantFields;
    }
}
