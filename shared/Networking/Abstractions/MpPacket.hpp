#pragma once
#include "custom-types/shared/macros.hpp"

#include "LiteNetLib/Utils/NetDataReader.hpp"
#include "LiteNetLib/Utils/NetDataWriter.hpp"
#include "LiteNetLib/Utils/INetSerializable.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES_DLL(MultiplayerCore::Networking::Abstractions, MpPacket, Il2CppObject, { classof(LiteNetLib::Utils::INetSerializable*) }, "MultiplayerCore.Networking.Abstractions",

    DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&LiteNetLib::Utils::INetSerializable::Serialize>::get(), LiteNetLib::Utils::NetDataWriter* writer);
    DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&LiteNetLib::Utils::INetSerializable::Deserialize>::get(), LiteNetLib::Utils::NetDataReader* reader);

    public:
        [[nodiscard]] LiteNetLib::Utils::INetSerializable* i_INetSerializable() { return reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(this); }
)

#include "beatsaber-hook/shared/utils/il2cpp-utils-methods.hpp"
template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<void (MultiplayerCore::Networking::Abstractions::MpPacket::*)(LiteNetLib::Utils::NetDataWriter*)>(&MultiplayerCore::Networking::Abstractions::MpPacket::Serialize)> {
    static const MethodInfo* get() {
        static auto* writer = &::il2cpp_utils::GetClassFromName("MultiplayerCore.Networking.Abstractions", "MpPacket")->byval_arg;
        return ::il2cpp_utils::FindMethod(classof(MultiplayerCore::Networking::Abstractions::MpPacket*), "Serialize", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{writer});
    }
};
template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<void (MultiplayerCore::Networking::Abstractions::MpPacket::*)(LiteNetLib::Utils::NetDataReader*)>(&MultiplayerCore::Networking::Abstractions::MpPacket::Deserialize)> {
    static const MethodInfo* get() {
        static auto* reader = &::il2cpp_utils::GetClassFromName("MultiplayerCore.Networking.Abstractions", "MpPacket")->byval_arg;
        return ::il2cpp_utils::FindMethod(classof(MultiplayerCore::Networking::Abstractions::MpPacket*), "Deserialize", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{reader});
    }
};
