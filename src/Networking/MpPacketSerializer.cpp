#include "Networking/MpPacketSerializer.hpp"
#include "logging.hpp"

#include "System/Type.hpp"
DEFINE_TYPE(MultiplayerCore::Networking, MpPacketSerializer);

namespace MultiplayerCore::Networking {
    void MpPacketSerializer::ctor(GlobalNamespace::IMultiplayerSessionManager* sessionManager) {
        INVOKE_CTOR();
        _sessionManager = il2cpp_utils::try_cast<GlobalNamespace::MultiplayerSessionManager>(sessionManager).value_or(nullptr);
    }

    void MpPacketSerializer::Initialize() {
        _sessionManager->RegisterSerializer(GlobalNamespace::MultiplayerSessionManager::MessageType(ID), this->i_INetworkPacketSubSerializer_1_IConnectedPlayer());
    }

    void MpPacketSerializer::Dispose() {
        _sessionManager->UnregisterSerializer(GlobalNamespace::MultiplayerSessionManager::MessageType(ID), this->i_INetworkPacketSubSerializer_1_IConnectedPlayer());
    }

    void MpPacketSerializer::Serialize(LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet) {
        auto packetType = reinterpret_cast<System::Object*>(packet)->GetType();
        writer->Put(packetType->NameOrDefault);
        packet->Serialize(writer);
    }

    void MpPacketSerializer::Deserialize(LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data) {
        std::string packetId = "null";
        auto prevPosition = reader->get_Position();
        try {
            packetId = static_cast<std::string>(reader->GetString());
            length -= reader->get_Position() - prevPosition;
            prevPosition = reader->get_Position();

            auto handlerItr = packetHandlers.find(packetId);
            if (handlerItr != packetHandlers.end() && handlerItr->second) {
                handlerItr->second(reader, length, data);
            }
        }
        catch (const std::exception& e) {
            std::string user;
			if (data) {
				if (data->get_userName())
					user += static_cast<std::string>(data->get_userName());
				if (!user.empty() && data->get_userId()) user += "|";
				if (data->get_userId())
					user += static_cast<std::string>(data->get_userId());
			}
            WARNING("An exception was thrown processing packet from player {}, packet {}: {}", user, packetId, e.what());
        }
        int processedBytes = reader->get_Position() - prevPosition;
        reader->SkipBytes(length - processedBytes);
    }

    bool MpPacketSerializer::HandlesType(Il2CppReflectionType* type) {
        return std::find(registeredTypes.begin(), registeredTypes.end(), type) != registeredTypes.end();
    }
}
