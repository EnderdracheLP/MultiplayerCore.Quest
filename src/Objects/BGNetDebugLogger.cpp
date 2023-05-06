#include "Objects/BGNetDebugLogger.hpp"
#include "logging.hpp"

DEFINE_TYPE(MultiplayerCore::Objects, BGNetDebugLogger);

namespace MultiplayerCore::Objects {
    void BGNetDebugLogger::ctor() {
        INVOKE_CTOR();
        BGNet::Logging::Debug::AddLogger(this->i_ILogger());
    }

    void BGNetDebugLogger::LogInfo(::StringW message) {
        INFO("{}", message);
    }

    void BGNetDebugLogger::LogError(::StringW message) {
        ERROR("{}", message);
    }

    void BGNetDebugLogger::LogException(::System::Exception* exception, ::StringW message) {
        if (message) {
            ERROR("{}", message);
        }
        ERROR("{}", exception->ToString());
    }

    void BGNetDebugLogger::LogWarning(::StringW message) {
        WARNING("{}", message);
    }
}
