#pragma once

#include "custom-types/shared/macros.hpp"
#include "GlobalNamespace/INetworkConfig.hpp"
#include "System/IDisposable.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerCore::Patchers, ModeSelectionPatcher, System::Object, classof(System::IDisposable*),
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::INetworkConfig*, _networkConfig);
    DECLARE_INSTANCE_FIELD_PRIVATE(StringW, lastStatusUrl);

    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
    DECLARE_CTOR(ctor, GlobalNamespace::INetworkConfig* networkConfig);

    DECLARE_INSTANCE_METHOD(bool, UpdateStatusUrl);
    DECLARE_INSTANCE_METHOD(StringW, get_lastStatusUrl);
    public:

        static ModeSelectionPatcher* get_instance();
    private:
        static ModeSelectionPatcher* instance;
)
