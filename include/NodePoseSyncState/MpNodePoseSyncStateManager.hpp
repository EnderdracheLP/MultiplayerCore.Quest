#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "GlobalNamespace/NodePoseSyncStateManager.hpp"
#include "MpNodePoseSyncStatePacket.hpp"
#include "Networking/MpPacketSerializer.hpp"
#include "System/IDisposable.hpp"
#include "Zenject/IInitializable.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerCore::NodePoseSyncState, MpNodePoseSyncStateManager, GlobalNamespace::NodePoseSyncStateManager, ::Zenject::IInitializable*, System::IDisposable*) {
    DECLARE_INSTANCE_FIELD_PRIVATE(Networking::MpPacketSerializer*, _packetSerializer);
    DECLARE_INSTANCE_FIELD_PRIVATE(long, deltaUpdateFrequencyMs);
    DECLARE_INSTANCE_FIELD_PRIVATE(long, fullStateUpdateFrequencyMs);

    DECLARE_OVERRIDE_METHOD_MATCH(long, get_deltaUpdateFrequencyMs, &GlobalNamespace::NodePoseSyncStateManager::get_deltaUpdateFrequencyMs);
    DECLARE_OVERRIDE_METHOD_MATCH(long, get_fullStateUpdateFrequencyMs, &GlobalNamespace::NodePoseSyncStateManager::get_fullStateUpdateFrequencyMs);

    DECLARE_INJECT_METHOD(void, Inject, Networking::MpPacketSerializer* packetSerializer);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Initialize, &Zenject::IInitializable::Initialize);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &System::IDisposable::Dispose);

    DECLARE_CTOR(ctor);

    public:
        void HandleNodePoseSyncUpdateReceived(MpNodePoseSyncStatePacket* data, GlobalNamespace::IConnectedPlayer* player);
};