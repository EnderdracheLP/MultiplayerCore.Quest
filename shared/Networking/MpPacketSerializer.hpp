#pragma once
#include "main.hpp"
#include "Callback.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include <vector>
#include "System/Action_3.hpp"
#include "LiteNetLib/Utils/NetDataWriter.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Generic/List_1.hpp"
#include "GlobalNamespace/INetworkPacketSubSerializer_1.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "GlobalNamespace/MultiplayerSessionManager.hpp"
#include "System/Type.hpp"

#include "GlobalNamespace/MultiplayerSessionManager_MessageType.hpp"

#include "Abstractions/MpPacket.hpp"
#include "logging.hpp"

using CallbackDictionary = std::unordered_map<std::string, MultiplayerCore::CallbackBase*>;

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerCore::Networking, MpPacketSerializer, Il2CppObject,
    classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*),
    DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*), "Serialize", 2), LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet);
    DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*), "Deserialize", 3), LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data);
    DECLARE_OVERRIDE_METHOD(bool, HandlesType, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*), "HandlesType", 1), Il2CppReflectionType* type);

    DECLARE_INSTANCE_FIELD(GlobalNamespace::MultiplayerSessionManager*, _sessionManager);

    DECLARE_CTOR(Construct, GlobalNamespace::MultiplayerSessionManager* sessionManager);
    DECLARE_DTOR(Deconstruct);

    private:
        static constexpr uint8_t Packet_ID = 100u;

        CallbackDictionary packetHandlers;

    public:
        template <class TPacket, ::il2cpp_utils::CreationType creationType = ::il2cpp_utils::CreationType::Temporary>
        void RegisterCallback(CallbackWrapper<TPacket, creationType>* callback) {
            static_assert(std::is_convertible_v<TPacket, LiteNetLib::Utils::INetSerializable*> || std::is_convertible_v<TPacket, MultiplayerCore::Networking::Abstractions::MpPacket*>, "Make sure your Type Implements and is Convertible to LiteNetLib::Utils::INetSerializable*");
            packetHandlers[static_cast<std::string>(csTypeOf(TPacket)->get_Name())] = callback;
        }

        template <class TPacket>
        void UnregisterCallback() {
            static_assert(std::is_convertible_v<TPacket, LiteNetLib::Utils::INetSerializable*> || std::is_convertible_v<TPacket, MultiplayerCore::Networking::Abstractions::MpPacket*>, "Make sure your Type Implements and is Convertible to LiteNetLib::Utils::INetSerializable*");
            DEBUG("UnregisterCallback called");

            auto itr = packetHandlers.find(static_cast<std::string>(csTypeOf(TPacket)->get_Name()));
            if (itr != packetHandlers.end()) {
                delete itr->second;
                packetHandlers.erase(itr);
            }
        }

        template <class TPacket>
        void Send(TPacket message) {
            static_assert(std::is_convertible_v<TPacket, LiteNetLib::Utils::INetSerializable*> || std::is_convertible_v<TPacket, MultiplayerCore::Networking::Abstractions::MpPacket*>, "Make sure your Type Implements and is Convertible to LiteNetLib::Utils::INetSerializable*");          
            try {
                if (_sessionManager) {
                    _sessionManager->Send(reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(message));
                }
                else {
                    CRITICAL("REPORT IMMEDIATELY: _sessionManager is null, THIS should NEVER happen, this mod CANNOT work this way");
                }
            }
            catch(il2cpp_utils::exceptions::StackTraceException const& e) {
                WARNING("An exception was thrown sending packet");
                ERROR("REPORT TO ENDER: {}", e.what());
                e.log_backtrace();
            }
            catch (il2cpp_utils::RunMethodException const& e) {
                WARNING("An exception was thrown sending packet");
                ERROR("REPORT TO ENDER: {}", e.what());
                e.log_backtrace();
            }
            catch(std::exception const& e) {
                WARNING("An exception was thrown sending packet");
                ERROR("REPORT TO ENDER: {}", e.what());
            }
            catch(...) {
                ERROR("An unknown exception was thrown sending packet");
            }
        }

        template <class TPacket>
        void SendUnreliable(TPacket message) {
            static_assert(std::is_convertible_v<TPacket, LiteNetLib::Utils::INetSerializable*> || std::is_convertible_v<TPacket, MultiplayerCore::Networking::Abstractions::MpPacket*>, "Make sure your Type Implements and is Convertible to LiteNetLib::Utils::INetSerializable*");
            try {
                if (_sessionManager) {
                    _sessionManager->SendUnreliable(reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(message));
                }
                else {
                    CRITICAL("REPORT IMMEDIATELY: _sessionManager is null, THIS should NEVER happen, this mod CANNOT work this way");
                }
            }
            catch(il2cpp_utils::exceptions::StackTraceException const& e) {
                WARNING("An exception was thrown sending packet");
                ERROR("REPORT TO ENDER: {}", e.what());
                e.log_backtrace();
            }
            catch (il2cpp_utils::RunMethodException const& e) {
                WARNING("An exception was thrown sending packet");
                ERROR("REPORT TO ENDER: {}", e.what());
                e.log_backtrace();
            }
            catch(std::exception const& e) {
                WARNING("An exception was thrown sending packet");
                ERROR("REPORT TO ENDER: {}", e.what());
            }
            catch(...) {
                ERROR("An unknown exception was thrown sending packet");
            }
        }

        /*
        Registers a Packet Callback
        */
        template <class TPacket, ::il2cpp_utils::CreationType creationType = ::il2cpp_utils::CreationType::Temporary>
        void RegisterCallback(PacketCallback<TPacket, creationType> callback) {
            static_assert(std::is_convertible_v<TPacket, LiteNetLib::Utils::INetSerializable*> || std::is_convertible_v<TPacket, MultiplayerCore::Networking::Abstractions::MpPacket*>, "Make sure your Type Implements and is Convertible to LiteNetLib::Utils::INetSerializable*");
            CallbackWrapper<TPacket, creationType>* newCallback = new CallbackWrapper<TPacket, creationType>(callback);
            this->RegisterCallback(newCallback);
        }
)
namespace MultiplayerCore {
    extern MultiplayerCore::Networking::MpPacketSerializer* mpPacketSerializer;
}