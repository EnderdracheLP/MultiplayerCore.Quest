#pragma once
#include "custom-types/shared/macros.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"


#include "Networking/Abstractions/MpPacket.hpp"

namespace MultiplayerCore::Players {
    enum class Platform
    {
        Unknown = 0,
        Steam = 1,
        OculusPC = 2,
        OculusQuest = 3,
        PS4 = 4
    };
}

//#define MPDInterfaces { classof(MultiplayerCore::MpPacket*) }
___DECLARE_TYPE_WRAPPER_INHERITANCE(MultiplayerCore::Players, MpPlayerData, Il2CppTypeEnum::IL2CPP_TYPE_CLASS,
    Il2CppObject, "MultiplayerCore.Players",
    classof(MultiplayerCore::MpPacket*), 0, nullptr,

    DECLARE_CTOR(New);

// Player and platform information
DECLARE_INSTANCE_FIELD(Il2CppString*, platformId);

DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::FindMethodUnsafe(classof(MultiplayerCore::Networking::Abstractions::MpPacket*), "Serialize", 1), LiteNetLib::Utils::NetDataWriter* writer);
DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::FindMethodUnsafe(classof(MultiplayerCore::Networking::Abstractions::MpPacket*), "Deserialize", 1), LiteNetLib::Utils::NetDataReader* reader);

    public:
        Platform platform;

        static MpPlayerData* Init(Il2CppString* platformID, Platform platform);
)
//#undef MPDInterfaces