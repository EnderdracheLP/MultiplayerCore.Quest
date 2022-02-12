#pragma once
#include "custom-types/shared/macros.hpp"

#include "LiteNetLib/Utils/NetDataReader.hpp"
#include "LiteNetLib/Utils/NetDataWriter.hpp"
#include "LiteNetLib/Utils/INetSerializable.hpp"

namespace MultiplayerCore::Networking::Abstractions {
    DECLARE_CLASS_CODEGEN_INTERFACES_DLL(MultiplayerCore::Networking::Abstractions, MpPacket, Il2CppObject, classof(LiteNetLib::Utils::INetSerializable*), "MultiplayerCore.Networking.Abstractions",
        
        DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::FindMethodUnsafe(classof(LiteNetLib::Utils::INetSerializable*), "Serialize", 1), LiteNetLib::Utils::NetDataWriter* writer);
        DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::FindMethodUnsafe(classof(LiteNetLib::Utils::INetSerializable*), "Deserialize", 1), LiteNetLib::Utils::NetDataReader* reader);

    )
}