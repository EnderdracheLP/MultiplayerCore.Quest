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
#include "System/Type.hpp"

#include "GlobalNamespace/MultiplayerSessionManager_MessageType.hpp"


using CallbackDictionary = std::map<std::string, MultiQuestensions::CallbackBase*>;
using TypeDictionary = std::map<Il2CppReflectionType*, std::string>;

DECLARE_CLASS_INTERFACES(MultiplayerCore::Networking, MpPacketSerializer, "System", "Object", sizeof(Il2CppObject),
    classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*),
    DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*), "Serialize", 2), LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet);
    DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*), "Deserialize", 3), LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data);
    DECLARE_OVERRIDE_METHOD(bool, HandlesType, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*), "HandlesType", 1), Il2CppReflectionType* type);

    //DECLARE_INSTANCE_METHOD(void, Serialize, LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet);
    //DECLARE_INSTANCE_METHOD(void, Deserialize, LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data);
    //DECLARE_INSTANCE_METHOD(bool, HandlesType, Il2CppReflectionType* type);

    DECLARE_INSTANCE_FIELD(MultiplayerSessionManager*, _sessionManager)

    DECLARE_CTOR(Construct);
    DECLARE_DTOR(Deconstruct);

    private:
        const int ID = 100;

        CallbackDictionary packetHandlers;
        TypeDictionary registeredTypes;

    public:
        template <class TPacket>
        void RegisterCallback(std::string identifier, CallbackWrapper<TPacket>* callback) {
            registeredTypes[csTypeOf(TPacket)] = identifier;
            packetHandlers[identifier] = callback;
        }

        template <class TPacket>
        void UnregisterCallback() {
            getLogger().debug("UnregisterCallback called");

            auto it = registeredTypes.find(csTypeOf(TPacket));
            if (it != registeredTypes.end()) registeredTypes.erase(it);

            auto itr = packetHandlers.find(csTypeOf(TPacket)->ToString());
            if (itr != packetHandlers.end()) {
                delete itr->second;
                packetHandlers.erase(itr);
            }
        }

        template <class TPacket>
        void RegisterCallback(std::string identifier, PacketCallback<TPacket> callback) {
            CallbackWrapper<TPacket>* newCallback = new CallbackWrapper<TPacket>(callback);
            if (identifier.empty()) {
                getLogger().error("Cannot register callback: Identifier null.");
                return;
            }
            else {
                packetSerializer->RegisterCallback(identifier, newCallback);
            }
        }

        template <class TPacket>
        void RegisterCallback(PacketCallback<TPacket> callback) {
            Il2CppReflectionType* packetType = csTypeOf(TPacket);
            if (packetType == nullptr) {
                getLogger().info("Packet Type null.");
                return;
            }

            std::string identifier = to_utf8(csstrtostr(packetType->ToString()));
            int pos = 0;
            while ((pos = identifier.find("::")) != std::string::npos)
            {
                identifier.replace(pos, 2, ".");
                pos++;
            }

            RegisterCallback<TPacket>(identifier, callback);
        }

        template <class TPacket>
        void UnregisterCallback() {
            packetSerializer->UnregisterCallback<TPacket>();
        }

        void UnregisterCallback(std::string identifier);
)