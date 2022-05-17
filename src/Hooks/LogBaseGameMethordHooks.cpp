#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "GlobalNamespace/SaberModelController.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;

namespace MultiplayerCore {

MAKE_HOOK_MATCH(SaberModelController_Init, &GlobalNamespace::SaberModelController::Init, void, GlobalNamespace::SaberModelController* self, UnityEngine::Transform* parent, GlobalNamespace::Saber* saber) {
    getLogger().debug("SaberModelControllerInit starting");
    SaberModelController_Init(self, parent, saber);
    if(!saber)
        getLogger().debug("SaberModelControllerInit, saber is not null");
}

    void Hooks::LogBaseGameHooks() {

        INSTALL_HOOK_ORIG(getLogger(), SaberModelController_Init);
    }
}