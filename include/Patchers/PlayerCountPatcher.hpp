#pragma once

#include "custom-types/shared/macros.hpp"
#include "GlobalNamespace/INetworkConfig.hpp"
#include "System/IDisposable.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerCore::Patchers, PlayerCountPatcher, Il2CppObject, classof(System::IDisposable*),
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::INetworkConfig*, _networkConfig);

    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());
    DECLARE_CTOR(ctor, GlobalNamespace::INetworkConfig* networkConfig);

    DECLARE_INSTANCE_FIELD_PRIVATE(bool, addEmptyPlayerSlotForEvenCount);
    DECLARE_INSTANCE_METHOD(bool, get_AddEmptyPlayerSlotForEvenCount);
    DECLARE_INSTANCE_METHOD(void, set_AddEmptyPlayerSlotForEvenCount, bool value);

    DECLARE_INSTANCE_METHOD(int, get_MaxPlayers);

    DECLARE_INSTANCE_FIELD_PRIVATE(int, minPlayers);
    DECLARE_INSTANCE_METHOD(int, get_MinPlayers);
    DECLARE_INSTANCE_METHOD(void, set_MinPlayers, int value);
    public:

        static PlayerCountPatcher* get_instance();
    private:
        static PlayerCountPatcher* instance;
)
