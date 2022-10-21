#include "Installers/AppInstaller.hpp"
#include "logging.hpp"

DEFINE_TYPE(MultiplayerCore::Installers, MpAppInstaller);

namespace MultiplayerCore::Installers {
    void MpAppInstaller::InstallBindings() {
        auto container = get_Container();
        INFO("Container: {}", container);
    };
}