#include "Networking/MpPacketSerializer.hpp"
#include "GlobalNamespace/BeatmapIdentifierNetSerializable.hpp"

#include "System/Array.hpp"

DEFINE_TYPE(MultiplayerCore::Networking, MpPacketSerializer);

namespace MultiplayerCore::Networking {


	void MpPacketSerializer::Construct(GlobalNamespace::MultiplayerSessionManager* sessionManager) {
		getLogger().debug("Constructing MpPacketSerializer");
		INVOKE_CTOR();
		this->_sessionManager = sessionManager;
		getLogger().debug("Registering MpPacketSerializer");
		_sessionManager->RegisterSerializer((GlobalNamespace::MultiplayerSessionManager_MessageType)MpPacketSerializer::Packet_ID, reinterpret_cast<GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*>(this));
		getLogger().debug("MpPacketSerializer Registered Successfully");
	}

	void MpPacketSerializer::Deconstruct() {
		getLogger().debug("Deconstructing MpPacketSerializer");
		for (auto it = packetHandlers.begin(); it != packetHandlers.end(); it++) {
			delete it->second;
		}
		packetHandlers.clear();
		getLogger().debug("Unregistering MpPacketSerializer");
		_sessionManager->UnregisterSerializer((GlobalNamespace::MultiplayerSessionManager_MessageType)MpPacketSerializer::Packet_ID, reinterpret_cast<GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*>(this));
		_sessionManager = nullptr;
		MpPacketSerializer::~MpPacketSerializer();
	}

	void MpPacketSerializer::Serialize(LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet) {
		System::Type* packetType = reinterpret_cast<Il2CppObject*>(packet)->GetType()/*il2cpp_utils::GetSystemType(il2cpp_functions::object_get_class(reinterpret_cast<Il2CppObject*>(packet)))*/;
		getLogger().debug("Serialize packetType is: %s", static_cast<std::string>(packetType->get_Name()).c_str());
		//getLogger().debug("Registered types check: %s", registeredTypes[packetType].c_str());
		//writer->Put(il2cpp_utils::newcsstr(registeredTypes[packetType])); // TODO: Go back to sending actual PacketType, namespace issue should be fixed
		writer->Put(packetType->get_Name());
		getLogger().debug("Writer Put");
		getLogger().debug("Nullptr checking: packet: %p, writer: %p", packet, writer);
		packet->Serialize(writer);
		getLogger().debug("Serialize Finished");
	}

	void MpPacketSerializer::Deserialize(LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data) {
		getLogger().debug("PacketSerializer::Deserialize");
		int prevPosition = reader->get_Position();
		getLogger().debug("reader prevPosition: %d", prevPosition);
		try {
			std::string packetType = static_cast<std::string>(reader->GetString());
			getLogger().debug("packetType: %s", packetType.c_str());
			length -= reader->get_Position() - prevPosition;
			getLogger().debug("length: %d", length);
			prevPosition = reader->get_Position();
			if (packetHandlers.find(packetType) != packetHandlers.end()) {
				getLogger().debug("packetHandlers found PacketType, try Invoke");
				packetHandlers[packetType]->Invoke(reader, length, data);
			}
		}
		catch(il2cpp_utils::exceptions::StackTraceException e) {
			std::string user;
			if (data) {
				if (data->get_userName())
					user += to_utf8(csstrtostr(data->get_userName()));
				if (!user.empty() && data->get_userId()) user += "|";
				if (data->get_userId())
					user += to_utf8(csstrtostr(data->get_userId()));
			}
			getLogger().warning("An exception was thrown processing packet from player '%s'", user.c_str());
			getLogger().error("REPORT TO ENDER: %s", e.what());
			e.log_backtrace();
		}
		catch (il2cpp_utils::RunMethodException const& e) {
			std::string user;
			if (data) {
				if (data->get_userName())
					user += to_utf8(csstrtostr(data->get_userName()));
				if (!user.empty() && data->get_userId()) user += "|";
				if (data->get_userId())
					user += to_utf8(csstrtostr(data->get_userId()));
			}
			getLogger().warning("An exception was thrown processing packet from player '%s'", user.c_str());
			getLogger().error("REPORT TO ENDER: %s", e.what());
			e.log_backtrace();
		}
		catch (const std::exception& e) {
			std::string user;
			if (data) {
				if (data->get_userName())
					user += to_utf8(csstrtostr(data->get_userName()));
				if (!user.empty() && data->get_userId()) user += "|";
				if (data->get_userId())
					user += to_utf8(csstrtostr(data->get_userId()));
			}
			getLogger().warning("An exception was thrown processing packet from player '%s'", user.c_str());
			getLogger().error("REPORT TO ENDER: %s", e.what());
		}
		catch (...) {
			std::string user;
			if (data) {
				if (data->get_userName())
					user += to_utf8(csstrtostr(data->get_userName()));
				if (!user.empty() && data->get_userId()) user += "|";
				if (data->get_userId())
					user += to_utf8(csstrtostr(data->get_userId()));
			}
			getLogger().warning("REPORT TO ENDER: An Unknown exception was thrown processing packet from player '%s'", user.c_str());
		}
		// skip any unprocessed bytes (or rewind the reader if too many bytes were read)
		int processedBytes = reader->get_Position() - prevPosition;
		reader->SkipBytes(length - processedBytes);
	}

	bool MpPacketSerializer::HandlesType(Il2CppReflectionType* type) {
		return packetHandlers.find(static_cast<std::string>(type->get_Name())) != packetHandlers.end();
	}
}