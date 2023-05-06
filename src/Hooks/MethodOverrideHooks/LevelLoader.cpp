#include "hooking.hpp"
#include "logging.hpp"
#include "InvokingLock.hpp"

#include "Objects/MpLevelLoader.hpp"

using namespace MultiplayerCore::Objects;

// This file exists to override methods on MultiplayerLevelLoader for the MultiplayerCore.Objects.MpLevelLoader, since on quest methods are not transformed into virtual calls

std::list<GlobalNamespace::MultiplayerLevelLoader*> LoadLevel_executing;
// override LoadLevel
MAKE_AUTO_HOOK_ORIG_MATCH(MultiplayerLevelLoader_LoadLevel, &GlobalNamespace::MultiplayerLevelLoader::LoadLevel, void, GlobalNamespace::MultiplayerLevelLoader* self, GlobalNamespace::ILevelGameplaySetupData* gameplaySetupData, float initialStartTime) {
    INVOKE_LOCK(MultiplayerLevelLoader_LoadLevel);
    if (!lock) {
        MultiplayerLevelLoader_LoadLevel(self, gameplaySetupData, initialStartTime);
    } else {
        static auto customKlass = classof(MpLevelLoader*);
        if (self->klass == customKlass)
            reinterpret_cast<MpLevelLoader*>(self)->LoadLevel_override(gameplaySetupData, initialStartTime);
        else
            MultiplayerLevelLoader_LoadLevel(self, gameplaySetupData, initialStartTime);
    }
}

MAKE_AUTO_HOOK_ORIG_MATCH(MultiplayerLevelLoader_Tick, &GlobalNamespace::MultiplayerLevelLoader::Tick, void, GlobalNamespace::MultiplayerLevelLoader* self) {
    INVOKE_LOCK(MultiplayerLevelLoader_LoadLevel);
    if (!lock) {
        MultiplayerLevelLoader_Tick(self);
    } else {
        static auto customKlass = classof(MpLevelLoader*);
        if (self->klass == customKlass)
            reinterpret_cast<MpLevelLoader*>(self)->Tick_override();
        else
            MultiplayerLevelLoader_Tick(self);
    }
}
