#include "NodePoseSyncState/MpNodePoseSyncStateManager.hpp"

DEFINE_TYPE(MultiplayerCore::NodePoseSyncState, MpNodePoseSyncStateManager);

namespace MultiplayerCore::NodePoseSyncState {
    void MpNodePoseSyncStateManager::ctor() {
        INVOKE_BASE_CTOR(classof(GlobalNamespace::NodePoseSyncStateManager*));
        deltaUpdateFrequency = 0.01f;
        fullStateUpdateFrequency = 0.1f;
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
            deltaUpdateFrequency = data->deltaUpdateFrequency;
            fullStateUpdateFrequency= data->fullStateUpdateFrequency;
        }
    }

    float MpNodePoseSyncStateManager::get_deltaUpdateFrequency() { return deltaUpdateFrequency; }
    float MpNodePoseSyncStateManager::get_fullStateUpdateFrequency() { return fullStateUpdateFrequency; }
}
