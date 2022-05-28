#pragma once
#include "main.hpp"

#include "GlobalNamespace/ThreadStaticPacketPool_1.hpp"
#include "GlobalNamespace/PacketPool_1.hpp"
#include "LiteNetLib/Utils/NetDataReader.hpp"

template <class T>
using PacketCallback = function_ptr_t<void, T, GlobalNamespace::IConnectedPlayer*>;

namespace MultiplayerCore {
	class CallbackBase {
	public:
		virtual ~CallbackBase() {};
		virtual void Invoke(LiteNetLib::Utils::NetDataReader* reader, int size, GlobalNamespace::IConnectedPlayer* player) = 0;
	};

	template <class TPacket, ::il2cpp_utils::CreationType creationType = ::il2cpp_utils::CreationType::Temporary> class CallbackWrapper : public CallbackBase {
	private:
		PacketCallback<TPacket> action = nullptr;

	public:
		CallbackWrapper(PacketCallback<TPacket> callback) {
			action = callback;
		}

		void Invoke(LiteNetLib::Utils::NetDataReader* reader, int size, GlobalNamespace::IConnectedPlayer* player) {
			getLogger().debug("Running Invoke creating packet");
			TPacket packet;
			try {
				packet = il2cpp_utils::NewSpecific<TPacket, creationType>(); 
			}
			catch(il2cpp_utils::exceptions::StackTraceException e) {
				getLogger().debug("Exception Creating Packet CT");
				getLogger().error("REPORT TO ENDER: %s", e.what());
				e.log_backtrace();
			}
			catch (il2cpp_utils::RunMethodException e) {
				getLogger().debug("Exception Creating Packet CT");
				getLogger().error("REPORT TO ENDER: %s", e.what());
				e.log_backtrace();
			} 
			// The 2 catches below should realistically not ever be needed, 
		    // but I still have them, also I should probably use a single try catch throughout this method
			catch (const std::exception& e) {
				getLogger().debug("Exception Creating Packet CT");
				getLogger().error("REPORT TO ENDER: %s", e.what());
			}
			catch (...) {
				getLogger().debug("REPORT TO ENDER: Unknown exception Creating Packet CT");
			}
			if (packet == nullptr) {
				reader->SkipBytes(size);
			}
			else {
				try {
					packet->Deserialize(reader);
				}
				catch(il2cpp_utils::exceptions::StackTraceException e) {
					getLogger().debug("Exception Deserializing Packet");
					getLogger().error("REPORT TO ENDER: %s", e.what());
					e.log_backtrace();
				}
				catch (il2cpp_utils::RunMethodException e) {
					getLogger().debug("Exception Deserializing Packet");
					getLogger().error("REPORT TO ENDER: %s", e.what());
					e.log_backtrace();
				}
				catch (const std::exception& e) {
					getLogger().debug("Exception Deserializing Packet");
					getLogger().error("REPORT TO ENDER: %s", e.what());
				}
				catch (...) {
					getLogger().debug("REPORT TO ENDER: Unknown exception Deserializing Packet");
				}
			}
			if (action != nullptr) {
				try {
					action(packet, player);
				}
				catch(il2cpp_utils::exceptions::StackTraceException e) {
					getLogger().debug("Exception running action");
					getLogger().error("REPORT TO ENDER: %s", e.what());
					e.log_backtrace();
				}
				catch (il2cpp_utils::RunMethodException e) {
					getLogger().debug("Exception running action");
					getLogger().error("REPORT TO ENDER: %s", e.what());
					e.log_backtrace();
				}
				catch (const std::exception& e) {
					getLogger().error("Exception running action");
					getLogger().error("REPORT TO ENDER: %s", e.what());
				}
				catch (...) {
					getLogger().debug("REPORT TO ENDER: Unknown exception Deserializing Packet");
				}
			}
			if constexpr (creationType == ::il2cpp_utils::CreationType::Manual)
				gc_free_specific(packet);
		}
	};
}