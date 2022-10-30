#include "Networking/MpPacketSerializer.hpp"
#include "GlobalNamespace/BeatmapIdentifierNetSerializable.hpp"

#include "System/Array.hpp"

#include "logging.hpp"

DEFINE_TYPE(MultiplayerCore::Networking, MpPacketSerializer);

namespace MultiplayerCore::Networking {


	void MpPacketSerializer::Construct(GlobalNamespace::MultiplayerSessionManager* sessionManager) {
		DEBUG("Constructing MpPacketSerializer");
		INVOKE_CTOR();
		this->_sessionManager = sessionManager;
		DEBUG("Registering MpPacketSerializer");
		_sessionManager->RegisterSerializer((GlobalNamespace::MultiplayerSessionManager_MessageType)MpPacketSerializer::Packet_ID, reinterpret_cast<GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*>(this));
		DEBUG("MpPacketSerializer Registered Successfully");
	}

	void MpPacketSerializer::Deconstruct() {
		MultiplayerCore::mpPacketSerializer = nullptr;
		DEBUG("Deconstructing MpPacketSerializer");
		for (auto it = packetHandlers.begin(); it != packetHandlers.end(); it++) {
			delete it->second;
		}
		packetHandlers.clear();
		DEBUG("Unregistering MpPacketSerializer");
		_sessionManager->UnregisterSerializer((GlobalNamespace::MultiplayerSessionManager_MessageType)MpPacketSerializer::Packet_ID, reinterpret_cast<GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*>(this));
		_sessionManager = nullptr;
		MpPacketSerializer::~MpPacketSerializer();
	}

	void MpPacketSerializer::Serialize(LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet) {
		System::Type* packetType = reinterpret_cast<Il2CppObject*>(packet)->GetType()/*il2cpp_utils::GetSystemType(il2cpp_functions::object_get_class(reinterpret_cast<Il2CppObject*>(packet)))*/;
		DEBUG("Serialize packetType is: {}", static_cast<std::string>(packetType->get_Name()).c_str());
		//DEBUG("Registered types check: {}", registeredTypes[packetType].c_str());
		//writer->Put(il2cpp_utils::newcsstr(registeredTypes[packetType])); // TODO: Go back to sending actual PacketType, namespace issue should be fixed
		writer->Put(packetType->get_Name());
		DEBUG("Writer Put");
		DEBUG("Nullptr checking: packet: {}, writer: {}", packet, writer);
		packet->Serialize(writer);
		DEBUG("Serialize Finished");
	}

	void MpPacketSerializer::Deserialize(LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data) {
		DEBUG("PacketSerializer::Deserialize");
		int prevPosition = reader->get_Position();
		DEBUG("reader prevPosition: {}", prevPosition);
		try {
			std::string packetType = static_cast<std::string>(reader->GetString());
			DEBUG("packetType: {}", packetType);
			length -= reader->get_Position() - prevPosition;
			DEBUG("length: {}", length);
			prevPosition = reader->get_Position();
			if (packetHandlers.find(packetType) != packetHandlers.end()) {
				DEBUG("packetHandlers found PacketType, try Invoke");
				packetHandlers[packetType]->Invoke(reader, length, data);
			}
		}
		catch(il2cpp_utils::exceptions::StackTraceException e) {
			std::string user;
			if (data) {
				if (data->get_userName())
					user +=  StringW(data->get_userName());
				if (!user.empty() && data->get_userId()) user += "|";
				if (data->get_userId())
					user +=  StringW(data->get_userId());
			}
			WARNING("An exception was thrown processing packet from player {}", user);
			ERROR("REPORT TO ENDER: {}", e.what());
			e.log_backtrace();
		}
		catch (il2cpp_utils::RunMethodException const& e) {
			std::string user;
			if (data) {
				if (data->get_userName())
					user +=  StringW(data->get_userName());
				if (!user.empty() && data->get_userId()) user += "|";
				if (data->get_userId())
					user +=  StringW(data->get_userId());
			}
			WARNING("An exception was thrown processing packet from player {}", user);
			ERROR("REPORT TO ENDER: {}", e.what());
			e.log_backtrace();
		}
		catch (const std::exception& e) {
			std::string user;
			if (data) {
				if (data->get_userName())
					user +=  StringW(data->get_userName());
				if (!user.empty() && data->get_userId()) user += "|";
				if (data->get_userId())
					user +=  StringW(data->get_userId());
			}
			WARNING("An exception was thrown processing packet from player {}", user);
			ERROR("REPORT TO ENDER: {}", e.what());
		}
		catch (...) {
			std::string user;
			if (data) {
				if (data->get_userName())
					user += StringW(data->get_userName());
				if (!user.empty() && data->get_userId()) user += "|";
				if (data->get_userId())
					user += StringW(data->get_userId());
			}
			WARNING("REPORT TO ENDER: An Unknown exception was thrown processing packet from player {}", user);
		}
		// skip any unprocessed bytes (or rewind the reader if too many bytes were read)
		int processedBytes = reader->get_Position() - prevPosition;
		reader->SkipBytes(length - processedBytes);
	}

	bool MpPacketSerializer::HandlesType(Il2CppReflectionType* type) {
		return packetHandlers.find(static_cast<std::string>(type->get_Name())) != packetHandlers.end();
	}
}