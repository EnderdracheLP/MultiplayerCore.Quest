#pragma once

#include "custom-types/shared/macros.hpp"
#include "lapiz/shared/macros.hpp"
#include "GlobalNamespace/NetworkPlayerEntitlementChecker.hpp"
#include "GlobalNamespace/IMultiplayerSessionManager.hpp"
#include "GlobalNamespace/EntitlementsStatus.hpp"
#include "System/Collections/Concurrent/ConcurrentDictionary_2.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Threading/Tasks/TaskCompletionSource_1.hpp"

#include <future>
#include <unordered_map>
#include <string>

using EntitlementsStatusTask = ::System::Threading::Tasks::Task_1<::GlobalNamespace::EntitlementsStatus>;

DECLARE_CLASS_CODEGEN(MultiplayerCore::Objects, MpEntitlementChecker, GlobalNamespace::NetworkPlayerEntitlementChecker) {
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::IMultiplayerSessionManager*, _sessionManager);

    DECLARE_INJECT_METHOD(void, Inject, GlobalNamespace::IMultiplayerSessionManager* sessionManager);

    DECLARE_INSTANCE_METHOD(void, Start_override);
    DECLARE_INSTANCE_METHOD(void, OnDestroy_override);
    DECLARE_INSTANCE_METHOD(void, HandleGetIsEntitledToLevel_override, StringW userId, StringW levelId);
    DECLARE_INSTANCE_METHOD(EntitlementsStatusTask*, GetEntitlementStatus_override, StringW levelId);

    DECLARE_INSTANCE_METHOD(void, HandleSetIsEntitledToLevel, StringW userId, StringW levelId, GlobalNamespace::EntitlementsStatus entitlement);
    DECLARE_INSTANCE_METHOD(GlobalNamespace::EntitlementsStatus, GetKnownEntitlement, StringW userId, StringW levelId);

    DECLARE_CTOR(ctor);
    public:
        UnorderedEventCallback<std::string, std::string, GlobalNamespace::EntitlementsStatus> receivedEntitlementEvent;

    private:
        GlobalNamespace::EntitlementsStatus GetEntitlementStatus(std::string_view levelHash);
        std::unordered_map<std::string, std::unordered_map<std::string, GlobalNamespace::EntitlementsStatus>> _entitlementsDictionary;
        std::unordered_map<std::string, SafePtr<EntitlementsStatusTask>> _entitlementsTasks;
};
