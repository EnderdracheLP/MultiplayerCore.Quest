#include "hooking.hpp"
#include "logging.hpp"
#include "InvokingLock.hpp"

#include "GlobalNamespace/NetworkPlayerEntitlementChecker.hpp"
#include "GlobalNamespace/EntitlementsStatus.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "Objects/MpEntitlementChecker.hpp"

using namespace MultiplayerCore::Objects;

// This file exists to override methods on NetworkPlayerEntitlementChecker for the MultiplayerCore.Objects.MpEntitlementChecker, since on quest methods are not transformed into virtual calls

// override Start
MAKE_AUTO_HOOK_MATCH(NetworkPlayerEntitlementChecker_Start, &::GlobalNamespace::NetworkPlayerEntitlementChecker::Start, void, GlobalNamespace::NetworkPlayerEntitlementChecker* self) {
    INVOKE_LOCK(NetworkPlayerEntitlementChecker_Start);

    if (!lock) {
        NetworkPlayerEntitlementChecker_Start(self);
    } else {
        static auto customKlass = classof(MpEntitlementChecker*);
        if (self->klass == customKlass)
            reinterpret_cast<MpEntitlementChecker*>(self)->Start_override();
        else
            NetworkPlayerEntitlementChecker_Start(self);
    }
}
// override OnDestroy
MAKE_AUTO_HOOK_MATCH(NetworkPlayerEntitlementChecker_OnDestroy, &::GlobalNamespace::NetworkPlayerEntitlementChecker::OnDestroy, void, GlobalNamespace::NetworkPlayerEntitlementChecker* self) {
    INVOKE_LOCK(NetworkPlayerEntitlementChecker_OnDestroy);

    if (!lock) {
        NetworkPlayerEntitlementChecker_OnDestroy(self);
    } else {
        static auto customKlass = classof(MpEntitlementChecker*);
        if (self->klass == customKlass)
            reinterpret_cast<MpEntitlementChecker*>(self)->OnDestroy_override();
        else
            NetworkPlayerEntitlementChecker_OnDestroy(self);
    }
}

// override HandleGetIsEntitledToLevel
MAKE_AUTO_HOOK_MATCH(NetworkPlayerEntitlementChecker_HandleGetIsEntitledToLevel, &::GlobalNamespace::NetworkPlayerEntitlementChecker::HandleGetIsEntitledToLevel, void, GlobalNamespace::NetworkPlayerEntitlementChecker* self, StringW userId, StringW levelId) {
    INVOKE_LOCK(NetworkPlayerEntitlementChecker_HandleGetIsEntitledToLevel);

    if (!lock) {
        NetworkPlayerEntitlementChecker_HandleGetIsEntitledToLevel(self, userId, levelId);
    } else {
        static auto customKlass = classof(MpEntitlementChecker*);
        if (self->klass == customKlass)
            reinterpret_cast<MpEntitlementChecker*>(self)->HandleGetIsEntitledToLevel_override(userId, levelId);
        else
            NetworkPlayerEntitlementChecker_HandleGetIsEntitledToLevel(self, userId, levelId);
    }
}

// override GetEntitlementStatus
MAKE_AUTO_HOOK_MATCH(NetworkPlayerEntitlementChecker_GetEntitlementStatus, &::GlobalNamespace::NetworkPlayerEntitlementChecker::GetEntitlementStatus, ::System::Threading::Tasks::Task_1<::GlobalNamespace::EntitlementsStatus>*, GlobalNamespace::NetworkPlayerEntitlementChecker* self, StringW levelId) {
    INVOKE_LOCK(NetworkPlayerEntitlementChecker_GetEntitlementStatus);

    if (!lock) {
        return NetworkPlayerEntitlementChecker_GetEntitlementStatus(self, levelId);
    } else {
        static auto customKlass = classof(MpEntitlementChecker*);
        if (self->klass == customKlass)
            return reinterpret_cast<MpEntitlementChecker*>(self)->GetEntitlementStatus_override(levelId);
        else
            return NetworkPlayerEntitlementChecker_GetEntitlementStatus(self, levelId);
    }
}
