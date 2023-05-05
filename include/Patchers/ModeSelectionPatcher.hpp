#pragma once

#include "custom-types/shared/macros.hpp"
#include "GlobalNamespace/INetworkConfig.hpp"
#include "System/IDisposable.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerCore::Patchers, ModeSelectionPatcher, Il2CppObject, classof(System::IDisposable*),
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::INetworkConfig*, _networkConfig);
    DECLARE_INSTANCE_FIELD_PRIVATE(StringW, lastStatusUrl);

    DECLARE_OVERRIDE_METHOD(void, Dispose, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::System::IDisposable::Dispose>::get());
    DECLARE_CTOR(ctor, GlobalNamespace::INetworkConfig* networkConfig);

    DECLARE_INSTANCE_METHOD(bool, UpdateStatusUrl);
    DECLARE_INSTANCE_METHOD(StringW, get_lastStatusUrl);
    public:

        static ModeSelectionPatcher* get_instance();
    private:
        static ModeSelectionPatcher* instance;
)
