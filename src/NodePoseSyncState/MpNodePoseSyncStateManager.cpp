#include "NodePoseSyncState/MpNodePoseSyncStateManager.hpp"
#include "logging.hpp"
DEFINE_TYPE(MultiplayerCore::NodePoseSyncState, MpNodePoseSyncStateManager);

namespace MultiplayerCore::NodePoseSyncState {
    void MpNodePoseSyncStateManager::ctor() {
        INVOKE_CTOR();
        INVOKE_BASE_CTOR(classof(GlobalNamespace::NodePoseSyncStateManager*));
        deltaUpdateFrequencyMs = 10L;
        fullStateUpdateFrequencyMs = 100L;
    }

    void MpNodePoseSyncStateManager::Inject(Networking::MpPacketSerializer* packetSerializer) {
        _packetSerializer = packetSerializer;
    }

    void MpNodePoseSyncStateManager::Initialize() {
        _packetSerializer->RegisterCallback<MpNodePoseSyncStatePacket*>(
            std::bind(&MpNodePoseSyncStateManager::HandleNodePoseSyncUpdateReceived, this, std::placeholders::_1, std::placeholders::_2)
        );
    }

    void MpNodePoseSyncStateManager::Dispose() {
        _packetSerializer->UnregisterCallback<MpNodePoseSyncStatePacket*>();
    }

    void MpNodePoseSyncStateManager::HandleNodePoseSyncUpdateReceived(MpNodePoseSyncStatePacket* data, GlobalNamespace::IConnectedPlayer* player) {
        if (player->get_isConnectionOwner()) {
            DEBUG("Updating node pose sync frequency to following values: delta: {}ms, full: {}ms", data->deltaUpdateFrequencyMs, data->fullStateUpdateFrequencyMs);
            bool shouldForceUpdate = deltaUpdateFrequencyMs != data->deltaUpdateFrequencyMs || fullStateUpdateFrequencyMs != data->fullStateUpdateFrequencyMs;
            deltaUpdateFrequencyMs = data->deltaUpdateFrequencyMs;
            fullStateUpdateFrequencyMs= data->fullStateUpdateFrequencyMs;
            if (shouldForceUpdate) {
                DEBUG("Forcing new state buffer update");
                _localState = nullptr;
            }
        }
    }

    long MpNodePoseSyncStateManager::get_deltaUpdateFrequencyMs() { 
        DEBUG("Returning delta update frequency: {}ms", deltaUpdateFrequencyMs);
        return deltaUpdateFrequencyMs; 
    }
    long MpNodePoseSyncStateManager::get_fullStateUpdateFrequencyMs() { 
        DEBUG("Returning full state update frequency: {}ms", fullStateUpdateFrequencyMs);
        return fullStateUpdateFrequencyMs; 
    }
}
