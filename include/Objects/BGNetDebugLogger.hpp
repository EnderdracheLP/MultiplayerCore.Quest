#pragma once

#include "custom-types/shared/macros.hpp"
#include "BGNet/Logging/Debug.hpp"
#include "BGNet/Logging/Debug_ILogger.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerCore::Objects, BGNetDebugLogger, Il2CppObject, classof(BGNet::Logging::Debug::ILogger*),
    DECLARE_OVERRIDE_METHOD(void, LogInfo, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::BGNet::Logging::Debug::ILogger::LogInfo>::get(), ::StringW message);
    DECLARE_OVERRIDE_METHOD(void, LogError, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::BGNet::Logging::Debug::ILogger::LogError>::get(), ::StringW message);
    DECLARE_OVERRIDE_METHOD(void, LogException, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::BGNet::Logging::Debug::ILogger::LogException>::get(), ::System::Exception* exception, ::StringW message);
    DECLARE_OVERRIDE_METHOD(void, LogWarning, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::BGNet::Logging::Debug::ILogger::LogWarning>::get(), ::StringW message);
    DECLARE_CTOR(ctor);

    public:
        BGNet::Logging::Debug::ILogger* i_ILogger() { return reinterpret_cast<BGNet::Logging::Debug::ILogger*>(this); }
)
