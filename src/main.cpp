#include "main.hpp"
#include "logging.hpp"

#include "pinkcore/shared/API.hpp"

#include "lapiz/shared/zenject/Zenjector.hpp"
#include "lapiz/shared/zenject/Location.hpp"

#include "custom-types/shared/register.hpp"

#include "Installers/AppInstaller.hpp"
#include "Installers/MenuInstaller.hpp"

/*
 * Something regarding guard clauses;
 * try to keep them to line 0, as they're
 * going to be easier to keep track of, and lets you change them easier.
 */

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
// other config tools such as config-utils don't use this config, so it can be removed if those are in use
Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}

// Returns a logger, useful for printing debug messages
// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load();
    INFO("Completed setup!");
}

using namespace Lapiz::Zenject;
using namespace MultiplayerCore;
// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();
    custom_types::Register::AutoRegister();

    auto zenjector = Zenjector::Get();

#pragma region PinkCore

#if defined(IGNORE_MOD_REQUIREMENTS)
#warning "Ignoring mod requirements"
    PinkCore::RequirementAPI::RegisterInstalled("Chroma");
    PinkCore::RequirementAPI::RegisterInstalled("Chroma Lighting Events");
    PinkCore::RequirementAPI::RegisterInstalled("Mapping Extensions");
    PinkCore::RequirementAPI::RegisterInstalled("Noodle Extensions");
#endif

#pragma endregion
#pragma region Hooks and Zenject
    DEBUG("Installing hooks and binding zenject stuff...");
    zenjector->Install<Installers::AppInstaller*>(Location::App);
    zenjector->Install<Installers::MenuInstaller*>(Location::Menu);
#pragma endregion
}