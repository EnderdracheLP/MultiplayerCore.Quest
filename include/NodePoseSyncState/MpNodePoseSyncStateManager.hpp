#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"

#include "GlobalNamespace/NodePoseSyncStateManager.hpp"
#include "MpNodePoseSyncStatePacket.hpp"
#include "Networking/MpPacketSerializer.hpp"
#include "System/IDisposable.hpp"
#include "Zenject/IInitializable.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerCore::NodePoseSyncState, MpNodePoseSyncStateManager, GlobalNamespace::NodePoseSyncStateManager, std::vector<Il2CppClass*>({classof(Zenject::IInitializable*), classof(System::IDisposable*)}),
    DECLARE_INSTANCE_FIELD_PRIVATE(Networking::MpPacketSerializer*, _packetSerializer);
    DECLARE_INSTANCE_FIELD_PRIVATE(float, deltaUpdateFrequency);
    DECLARE_INSTANCE_FIELD_PRIVATE(float, fullStateUpdateFrequency);

    DECLARE_OVERRIDE_METHOD(float, get_deltaUpdateFrequency, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::NodePoseSyncStateManager::get_deltaUpdateFrequency>::get());
    DECLARE_OVERRIDE_METHOD(float, get_fullStateUpdateFrequency, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::NodePoseSyncStateManager::get_fullStateUpdateFrequency>::get());

    DECLARE_INJECT_METHOD(void, Inject, Networking::MpPacketSerializer* packetSerializer);

    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Zenject::IInitializable::Initialize>::get());
    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&System::IDisposable::Dispose>::get());

    DECLARE_CTOR(ctor);

    public:
        void HandleNodePoseSyncUpdateReceived(MpNodePoseSyncStatePacket* data, GlobalNamespace::IConnectedPlayer* player);
)
