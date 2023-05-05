#pragma once

#include "custom-types/shared/macros.hpp"
#include "Zenject/IInitializable.hpp"
#include "System/IDisposable.hpp"

#include "../Networking/Abstractions/MpPacket.hpp"

namespace MultiplayerCore::Players {
    enum class Platform {
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

DECLARE_CLASS_CUSTOM_DLL(MultiplayerCore::Players, MpPlayerData, MultiplayerCore::Networking::Abstractions::MpPacket, "MultiplayerCore.Players",
    DECLARE_CTOR(New);

    DECLARE_INSTANCE_FIELD(StringW, platformId);
    DECLARE_INSTANCE_FIELD(Platform, platform);

    DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::LiteNetLib::Utils::INetSerializable::Serialize>::get(), LiteNetLib::Utils::NetDataWriter* writer);
    DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::LiteNetLib::Utils::INetSerializable::Deserialize>::get(), LiteNetLib::Utils::NetDataReader* reader);

    public:
        [[nodiscard("Attempting to discard manually created MpPlayerData")]] static MpPlayerData* Init(StringW platformID, Platform platform);
)
