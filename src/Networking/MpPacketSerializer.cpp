#include "Networking/MpPacketSerializer.hpp"

DEFINE_TYPE(MultiplayerCore::Networking, MpPacketSerializer);

namespace MultiplayerCore::Networking {
    void MpPacketSerializer::ctor(GlobalNamespace::IMultiplayerSessionManager* sessionManager) {
        _sessionManager = il2cpp_utils::try_cast<std::remove_pointer_t<decltype(_sessionManager)>>(sessionManager).value_or(nullptr);
    }

    void MpPacketSerializer::Initialize() {
        _sessionManager->RegisterSerializer(GlobalNamespace::MultiplayerSessionManager_MessageType(ID), this->i_INetworkPacketSubSerializer_1_IConnectedPlayer());
    }

    void MpPacketSerializer::Dispose() {
        _sessionManager->UnregisterSerializer(GlobalNamespace::MultiplayerSessionManager_MessageType(ID), this->i_INetworkPacketSubSerializer_1_IConnectedPlayer());
    }

    void MpPacketSerializer::Serialize(LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet) {
        auto packetType = reinterpret_cast<Il2CppObject*>(packet)->GetType();
        writer->Put(packetType->get_Name());
        packet->Serialize(writer);
    }

    void MpPacketSerializer::Deserialize(LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data) {
        auto prevPosition = reader->get_Position();
        std::string packetId(reader->GetString());
        length -= reader->get_Position() - prevPosition;
        prevPosition = reader->get_Position();

        auto handlerItr = packetHandlers.find(packetId);
        if (handlerItr != packetHandlers.end() && handlerItr->second) {
            handlerItr->second(reader, length, data);
        }

        int processedBytes = reader->get_Position() - prevPosition;
        reader->SkipBytes(length - processedBytes);
    }

    bool MpPacketSerializer::HandlesType(Il2CppReflectionType* type) {
        return std::find(registeredTypes.begin(), registeredTypes.end(), type) != registeredTypes.end();
    }
}
