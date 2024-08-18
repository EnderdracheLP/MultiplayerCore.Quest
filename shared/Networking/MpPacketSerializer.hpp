#pragma once

#include "custom-types/shared/macros.hpp"
#if __has_include("logging.hpp")
#include "logging.hpp"
#endif

#include "GlobalNamespace/INetworkPacketSubSerializer_1.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "LiteNetLib/Utils/NetDataWriter.hpp"
#include "LiteNetLib/Utils/NetDataReader.hpp"
#include "LiteNetLib/Utils/INetSerializable.hpp"
#include "GlobalNamespace/IPoolablePacket.hpp"

#include "System/IDisposable.hpp"
#include "System/Type.hpp"
#include "Zenject/IInitializable.hpp"

#include "GlobalNamespace/MultiplayerSessionManager.hpp"
#include "GlobalNamespace/ThreadStaticPacketPool_1.hpp"

#include <type_traits>
#include <map>
#include <list>

namespace MultiplayerCore {
    template<typename TPacket>
    concept INetSerializable = requires(TPacket t) {
        {t->i_INetSerializable()} -> std::same_as<LiteNetLib::Utils::INetSerializable*>;
    };

    template<typename TPacket>
    concept IPoolablePacket = INetSerializable<TPacket> && requires(TPacket t) {
        {t->i_IPoolablePacket()} -> std::same_as<GlobalNamespace::IPoolablePacket*>;
        {GlobalNamespace::ThreadStaticPacketPool_1<TPacket>::get_pool()->Obtain()} -> std::same_as<TPacket>;
    };
}

using PacketHandler = std::function<void (LiteNetLib::Utils::NetDataReader*, int, GlobalNamespace::IConnectedPlayer*)>;

template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<void (GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer *>::*)(LiteNetLib::Utils::NetDataWriter*, LiteNetLib::Utils::INetSerializable*)>(&GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer *>::Serialize)> {
    static const MethodInfo* methodInfo() {
        static auto klass = classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer *>*);
        static auto* writer = &classof(LiteNetLib::Utils::NetDataWriter*)->byval_arg;
        static auto* packet = &classof(LiteNetLib::Utils::INetSerializable*)->byval_arg;
        return ::il2cpp_utils::FindMethod(klass, "Serialize", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{writer, packet});
    }
};

template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<void (GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer *>::*)(LiteNetLib::Utils::NetDataReader*, int, GlobalNamespace::IConnectedPlayer*)>(&GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer *>::Deserialize)> {
    static const MethodInfo* methodInfo() {
        static auto klass = classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer *>*);
        static auto* reader = &classof(LiteNetLib::Utils::NetDataReader*)->byval_arg;
        static auto* length = &classof(int)->byval_arg;
        static auto* data = &classof(GlobalNamespace::IConnectedPlayer*)->byval_arg;

        return ::il2cpp_utils::FindMethod(klass, "Deserialize", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{reader, length, data});
    }
};

template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<bool (GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer *>::*)(System::Type*)>(&GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer *>::HandlesType)> {
    static const MethodInfo* methodInfo() {
        static auto klass = classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer *>*);
        static auto* type = &classof(System::Type*)->byval_arg;

        return ::il2cpp_utils::FindMethod(klass, "HandlesType", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{type});
    }
};

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerCore::Networking, MpPacketSerializer, System::Object,
    std::vector<Il2CppClass*>({classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*), classof(::Zenject::IInitializable*), classof(System::IDisposable*)}),

    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &::Zenject::IInitializable::Initialize);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Serialize, &GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>::Serialize, LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Deserialize, &GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>::Deserialize, LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data);
    DECLARE_OVERRIDE_METHOD_MATCH(bool, HandlesType, &GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>::HandlesType, Il2CppReflectionType* type);

    static const uint8_t ID = 100u;

    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::MultiplayerSessionManager*, _sessionManager);

    DECLARE_CTOR(ctor, GlobalNamespace::IMultiplayerSessionManager* sessionManager);

    public:
        GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>* i_INetworkPacketSubSerializer_1_IConnectedPlayer() { return reinterpret_cast<GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*>(this); }
        Zenject::IInitializable* i_IInitializable() { return reinterpret_cast<::Zenject::IInitializable*>(this); }
        System::IDisposable* i_IDisposable() { return reinterpret_cast<::System::IDisposable*>(this); }

        template<::MultiplayerCore::IPoolablePacket TPacket>
        requires(std::is_pointer_v<TPacket>)
        static TPacket ObtainPacket() {
            return GlobalNamespace::ThreadStaticPacketPool_1<TPacket>::get_pool()->Obtain();
        }

        template<::MultiplayerCore::INetSerializable TPacket>
        requires(std::is_pointer_v<TPacket> && !::MultiplayerCore::IPoolablePacket<TPacket>)
        static TPacket ObtainPacket() {
            return il2cpp_utils::NewSpecific<TPacket>();
        }

        template<::MultiplayerCore::INetSerializable TPacket>
        requires(std::is_pointer_v<TPacket>)
        static TPacket DeserializePacket(LiteNetLib::Utils::NetDataReader* reader, int size) {
            auto packet = ObtainPacket<TPacket>();
            if (!packet) {
                reader->SkipBytes(size);
            } else packet->Deserialize(reader);
            return packet;
        }

        template<::MultiplayerCore::INetSerializable TPacket>
        requires(std::is_pointer_v<TPacket>)
        void RegisterCallback(std::function<void(TPacket)> callback) { RegisterCallback([callback](TPacket packet, GlobalNamespace::IConnectedPlayer* player){ callback(packet); }); }

        template<::MultiplayerCore::INetSerializable TPacket>
        requires(std::is_pointer_v<TPacket>)
        void RegisterCallback(std::function<void(TPacket, GlobalNamespace::IConnectedPlayer*)> callback) {
            System::Type* packetType = csTypeOf(TPacket);
            registeredTypes.emplace_back(reinterpret_cast<Il2CppReflectionType*>(packetType));
            std::string packetId(packetType->NameOrDefault);

            packetHandlers[packetId] = [callback](LiteNetLib::Utils::NetDataReader* reader, int size, GlobalNamespace::IConnectedPlayer* player){
                callback(DeserializePacket<TPacket>(reader, size), player);
            };
        }

        template<::MultiplayerCore::INetSerializable TPacket>
        requires(std::is_pointer_v<TPacket>)
        void RegisterCallback(const std::string& packetId, std::function<void(TPacket)> callback) { RegisterCallback(packetId, [callback](TPacket packet, GlobalNamespace::IConnectedPlayer* player){ callback(packet); }); }

        template<::MultiplayerCore::INetSerializable TPacket>
        requires(std::is_pointer_v<TPacket>)
        void RegisterCallback(const std::string& packetId, std::function<void(TPacket, GlobalNamespace::IConnectedPlayer*)> callback) {
            Il2CppReflectionType* packetType = csTypeOf(TPacket);
            registeredTypes.emplace_back(reinterpret_cast<Il2CppReflectionType*>(packetType));

            packetHandlers[packetId] = [callback](LiteNetLib::Utils::NetDataReader* reader, int size, GlobalNamespace::IConnectedPlayer* player){
                callback(DeserializePacket<TPacket>(reader, size), player);
            };
        }

        template<::MultiplayerCore::INetSerializable TPacket>
        requires(std::is_pointer_v<TPacket>)
        void UnregisterCallback() {
            System::Type* packetType = csTypeOf(TPacket);
            std::string packetId(packetType->NameOrDefault);

            std::erase(registeredTypes, reinterpret_cast<Il2CppReflectionType*>(packetType));
            packetHandlers.erase(packetId);
        }

        template<::MultiplayerCore::INetSerializable TPacket>
        requires(std::is_pointer_v<TPacket>)
        void SendToPlayer(TPacket packet, GlobalNamespace::IConnectedPlayer* player) {
            /* TODO: try catch, logging? */
            if (_sessionManager) {
                _sessionManager->SendToPlayer(packet->i_INetSerializable(), player);
            }
        }


        template<::MultiplayerCore::INetSerializable TPacket>
        requires(std::is_pointer_v<TPacket>)
        void Send(TPacket packet) {
            /* TODO: try catch, logging? */
            if (_sessionManager) {
                _sessionManager->Send(packet->i_INetSerializable());
            }
        }

        template<::MultiplayerCore::INetSerializable TPacket>
        requires(std::is_pointer_v<TPacket>)
        void SendUnreliable(TPacket packet) {
            /* TODO: try catch, logging? */
            if (_sessionManager) {
                _sessionManager->SendUnreliable(packet->i_INetSerializable());
            }
        }

    private:
        std::list<Il2CppReflectionType*> registeredTypes;
        std::map<std::string, PacketHandler> packetHandlers;
)
