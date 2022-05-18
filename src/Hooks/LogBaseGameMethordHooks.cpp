#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "GlobalNamespace/SaberModelController.hpp"

//Does not cause blackscreen crash

using namespace GlobalNamespace;
using namespace UnityEngine;

namespace MultiplayerCore {

MAKE_HOOK_MATCH(SaberModelController_Init, &GlobalNamespace::SaberModelController::Init, void, GlobalNamespace::SaberModelController* self, UnityEngine::Transform* parent, GlobalNamespace::Saber* saber) {
    getLogger().debug("MPCore_SaberModelControllerInit starting");
    SaberModelController_Init(self, parent, saber);
    if(!saber)
        getLogger().debug("MPCore_SaberModelControllerInit, saber is not null");
}

    void Hooks::LogBaseGameHooks() {

        INSTALL_HOOK_ORIG(getLogger(), SaberModelController_Init);
    }
}