#include "logging.hpp"
#include "hooking.hpp"

#include "lapiz/shared/arrayutils.hpp"
#include "GlobalNamespace/MainSystemInit.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/ConcreteIdBinderGeneric_1.hpp"
#include "Zenject/ConcreteIdBinderNonGeneric.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"
#include "Objects/MpEntitlementChecker.hpp"

MAKE_AUTO_HOOK_ORIG_MATCH(MainSystemInit_InstallBindings, &::GlobalNamespace::MainSystemInit::InstallBindings, void, GlobalNamespace::MainSystemInit* self, Zenject::DiContainer* container) {
    MainSystemInit_InstallBindings(self, container);

    // TODO: check if this actually will just work
    container->Unbind<GlobalNamespace::NetworkPlayerEntitlementChecker*>();
    auto bindArr = Lapiz::ArrayUtils::TypeArray<GlobalNamespace::NetworkPlayerEntitlementChecker*>();
    auto toArr = Lapiz::ArrayUtils::TypeArray<MultiplayerCore::Objects::MpEntitlementChecker*>();
    container->Bind(bindArr)->To(toArr)->FromNewComponentOnRoot()->AsSingle()->NonLazy();
}
