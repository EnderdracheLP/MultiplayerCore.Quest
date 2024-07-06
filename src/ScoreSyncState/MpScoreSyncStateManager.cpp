#include "ScoreSyncState/MpScoreSyncStateManager.hpp"

DEFINE_TYPE(MultiplayerCore::ScoreSyncState, MpScoreSyncStateManager);

namespace MultiplayerCore::ScoreSyncState {
    void MpScoreSyncStateManager::ctor() {
        INVOKE_CTOR();
        INVOKE_BASE_CTOR(classof(GlobalNamespace::ScoreSyncStateManager*));
        deltaUpdateFrequencyMs = 10;
        fullStateUpdateFrequencyMs = 100;
    }

    void MpScoreSyncStateManager::Inject(Networking::MpPacketSerializer* packetSerializer) {
        _packetSerializer = packetSerializer;
    }

    void MpScoreSyncStateManager::Initialize() {
        _packetSerializer->RegisterCallback<MpScoreSyncStatePacket*>(
            std::bind(&MpScoreSyncStateManager::HandleScoreSyncUpdateReceived, this, std::placeholders::_1, std::placeholders::_2)
        );
    }

    void MpScoreSyncStateManager::Dispose() {
        _packetSerializer->UnregisterCallback<MpScoreSyncStatePacket*>();
    }

    void MpScoreSyncStateManager::HandleScoreSyncUpdateReceived(MpScoreSyncStatePacket* data, GlobalNamespace::IConnectedPlayer* player) {
        if (player->get_isConnectionOwner()) {
            deltaUpdateFrequencyMs = data->deltaUpdateFrequencyMs;
            fullStateUpdateFrequencyMs= data->fullStateUpdateFrequencyMs;
        }
    }

    long MpScoreSyncStateManager::get_deltaUpdateFrequencyMs() { return deltaUpdateFrequencyMs; }
    long MpScoreSyncStateManager::get_fullStateUpdateFrequencyMs() { return fullStateUpdateFrequencyMs; }
}
