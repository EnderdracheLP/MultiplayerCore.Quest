#pragma once

#include "../_config.h"
#include "custom-types/shared/macros.hpp"
#include "Zenject/IInitializable.hpp"
#include "System/IDisposable.hpp"

#include "../Networking/Abstractions/MpPacket.hpp"

namespace MultiplayerCore::Players {
    enum class MPCORE_EXPORT Platform {
        Unknown = 0,
        Steam = 1,
        OculusPC = 2,
        OculusQuest = 3,
        PS4 = 4
    };
}

// categorize the enum as an int for C# calls
template<>
struct ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<MultiplayerCore::Players::Platform> {
    static inline Il2CppClass* get() {
        return classof(int);
    }
};
static_assert(sizeof(MultiplayerCore::Players::Platform) == sizeof(int));

DECLARE_CLASS_CUSTOM(MultiplayerCore::Players, MpPlayerData, MultiplayerCore::Networking::Abstractions::MpPacket,
    DECLARE_CTOR(New);

    DECLARE_INSTANCE_FIELD(StringW, platformId);
    DECLARE_INSTANCE_FIELD(Platform, platform);
    DECLARE_INSTANCE_FIELD(StringW, gameVersion);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Serialize, &::LiteNetLib::Utils::INetSerializable::Serialize, LiteNetLib::Utils::NetDataWriter* writer);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Deserialize, &::LiteNetLib::Utils::INetSerializable::Deserialize, LiteNetLib::Utils::NetDataReader* reader);
)
