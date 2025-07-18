#include "_config.h"
#include "hooking.hpp"
#include "logging.hpp"

#include "tasks.hpp"

#include "custom-types/shared/register.hpp"

#include "Installers/MpAppInstaller.hpp"
#include "Installers/MpMenuInstaller.hpp"

#include "lapiz/shared/zenject/Zenjector.hpp"
#include "bsml/shared/BSML.hpp"

#define BEATSAVER_PLUSPLUS_AUTO_INIT

modloader::ModInfo modInfo{MOD_ID, VERSION, VERSION_LONG};

MPCORE_EXPORT_FUNC void setup(CModInfo* info) {
    info->id = MOD_ID;
    info->version = VERSION;
    info->version_long = VERSION_LONG;

    INFO("Mod '{}' with version '{}' version long '{}' finished setup", MOD_ID, VERSION, VERSION_LONG);
}

MPCORE_EXPORT_FUNC void late_load() {

    il2cpp_functions::Init();
    custom_types::Register::AutoRegister();
    MultiplayerCore::Hooking::InstallHooks();
    BSML::Init();

    auto zenjector = Lapiz::Zenject::Zenjector::Get();
    zenjector->Install<MultiplayerCore::Installers::MpAppInstaller*>(Lapiz::Zenject::Location::App);
    zenjector->Install<MultiplayerCore::Installers::MpMenuInstaller*>(Lapiz::Zenject::Location::Menu);

    INFO("Mod '{}' with version '{}' version long {} finished late load", MOD_ID, VERSION, VERSION_LONG);

    // TODO: Remove test thread
    DEBUG("Running Thread Test");
    auto testT = MultiplayerCore::StartTask<bool>([]() {
        try {
            DEBUG("Message from Thread Test");
        } catch (...) {}
        return true;
    });
    DEBUG("Started Thread Test");
}
