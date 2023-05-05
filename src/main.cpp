#include "hooking.hpp"
#include "logging.hpp"

#include "custom-types/shared/register.hpp"

#include "Installers/MpAppInstaller.hpp"
#include "Installers/MpMenuInstaller.hpp"

#include "lapiz/shared/zenject/Zenjector.hpp"
#include "bsml/shared/BSML.hpp"

ModInfo modInfo{MOD_ID, VERSION};

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

extern "C" void setup(ModInfo& info) {
    info = modInfo;
}

extern "C" void load() {
    auto& logger = getLogger();
    // TODO: load configs & server configs
    il2cpp_functions::Init();
    custom_types::Register::AutoRegister();
    Hooks::InstallHooks(logger);
    BSML::Init();

    auto zenjector = Lapiz::Zenject::Zenjector::Get();
    zenjector->Install<MultiplayerCore::Installers::MpAppInstaller*>(Lapiz::Zenject::Location::App);
    zenjector->Install<MultiplayerCore::Installers::MpMenuInstaller*>(Lapiz::Zenject::Location::Menu);
}
