#pragma once

#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "Zenject/IInitializable.hpp"
#include "System/IDisposable.hpp"
#include "MpPlayerData.hpp"

#include "../Networking/MpPacketSerializer.hpp"
#include "GlobalNamespace/IPlatformUserModel.hpp"
#include "GlobalNamespace/IMultiplayerSessionManager.hpp"
#include "GlobalNamespace/UserInfo.hpp"

#include "System/Collections/Concurrent/ConcurrentDictionary_2.hpp"

// TODO: check if this is even going to work because MpPlayerData is a custom class
using ConcurrentPlayerDataDictionary = System::Collections::Concurrent::ConcurrentDictionary_2<StringW, MultiplayerCore::Players::MpPlayerData*>;

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerCore::Players, MpPlayerManager, Il2CppObject, std::vector<Il2CppClass*>({classof(::Zenject::IInitializable*), classof(::System::IDisposable*)}),
    DECLARE_INSTANCE_FIELD_PRIVATE(MultiplayerCore::Networking::MpPacketSerializer*, _packetSerializer);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::IPlatformUserModel*, _platformUserModel);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::IMultiplayerSessionManager*, _sessionManager);
    DECLARE_INSTANCE_FIELD_PRIVATE(ConcurrentPlayerDataDictionary*, _playerData);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::UserInfo*, _localPlayerInfo);

    DECLARE_OVERRIDE_METHOD(void, Initialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::IInitializable::Initialize>::get());
    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());

    DECLARE_CTOR(ctor, MultiplayerCore::Networking::MpPacketSerializer* packetSerializer, GlobalNamespace::IPlatformUserModel* platformUserModel, GlobalNamespace::IMultiplayerSessionManager* sessionManager);
    DECLARE_INSTANCE_METHOD(void, HandlePlayerConnected, GlobalNamespace::IConnectedPlayer* player);
    DECLARE_INSTANCE_METHOD(void, HandlePlayerData, MpPlayerData* packet, GlobalNamespace::IConnectedPlayer* player);

    public:
        bool TryGetPlayer(StringW userId, MpPlayerData*& outplayer);
        MpPlayerData* GetPlayer(StringW userId);

        UnorderedEventCallback<GlobalNamespace::IConnectedPlayer*, MpPlayerData*> PlayerConnectedEvent;
        custom_types::Helpers::Coroutine AwaitUser();
)
