#pragma once

#include "custom-types/shared/macros.hpp"
#include "BGNet/Logging/Debug.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerCore::Objects, BGNetDebugLogger, System::Object, classof(BGNet::Logging::Debug::ILogger*),
    DECLARE_OVERRIDE_METHOD_MATCH(void, LogInfo, &::BGNet::Logging::Debug::ILogger::LogInfo, ::StringW message);
    DECLARE_OVERRIDE_METHOD_MATCH(void, LogError, &::BGNet::Logging::Debug::ILogger::LogError, ::StringW message);
    DECLARE_OVERRIDE_METHOD_MATCH(void, LogException, &::BGNet::Logging::Debug::ILogger::LogException, ::System::Exception* exception, ::StringW message);
    DECLARE_OVERRIDE_METHOD_MATCH(void, LogWarning, &::BGNet::Logging::Debug::ILogger::LogWarning, ::StringW message);
    DECLARE_CTOR(ctor);

    public:
        BGNet::Logging::Debug::ILogger* i_ILogger() { return reinterpret_cast<BGNet::Logging::Debug::ILogger*>(this); }
)
