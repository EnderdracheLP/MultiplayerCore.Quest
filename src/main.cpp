#include "_config.h"
#include "hooking.hpp"
#include "logging.hpp"

#include "custom-types/shared/register.hpp"

#include "Installers/MpAppInstaller.hpp"
#include "Installers/MpMenuInstaller.hpp"

#include "lapiz/shared/zenject/Zenjector.hpp"
#include "bsml/shared/BSML.hpp"

modloader::ModInfo modInfo{MOD_ID, VERSION, VERSION_LONG};

MPCORE_EXPORT_FUNC void setup(CModInfo* info) {
    info->id = MOD_ID;
    info->version = VERSION;
    info->version_long = VERSION_LONG;
}

MPCORE_EXPORT_FUNC void late_load() {

    il2cpp_functions::Init();
    custom_types::Register::AutoRegister();
    MultiplayerCore::Hooking::InstallHooks();
    BSML::Init();

    auto zenjector = Lapiz::Zenject::Zenjector::Get();
    zenjector->Install<MultiplayerCore::Installers::MpAppInstaller*>(Lapiz::Zenject::Location::App);
    zenjector->Install<MultiplayerCore::Installers::MpMenuInstaller*>(Lapiz::Zenject::Location::Menu);
}
