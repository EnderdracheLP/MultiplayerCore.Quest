#include "hooking.hpp"
#include "logging.hpp"

#include "Objects/MpLevelLoader.hpp"

#include "GlobalNamespace/MultiplayerMenuInstaller.hpp"
#include "GlobalNamespace/MultiplayerLevelLoader.hpp"

#include "Zenject/DiContainer.hpp"
#include "Zenject/ConcreteIdBinderNonGeneric.hpp"
#include "lapiz/shared/arrayutils.hpp"

MAKE_AUTO_HOOK_MATCH(MultiplayerMenuInstaller_InstallBindings, &::GlobalNamespace::MultiplayerMenuInstaller::InstallBindings, void, GlobalNamespace::MultiplayerMenuInstaller* self) {
    MultiplayerMenuInstaller_InstallBindings(self);

    auto container = self->get_Container();
    container->Unbind<GlobalNamespace::MultiplayerLevelLoader*>();
    auto bindarray = Lapiz::ArrayUtils::TypeArray<GlobalNamespace::MultiplayerLevelLoader*, MultiplayerCore::Objects::MpLevelLoader*, Zenject::ITickable*>();
    auto toarray = Lapiz::ArrayUtils::TypeArray<MultiplayerCore::Objects::MpLevelLoader*>();
    container->Bind(bindarray)->To(toarray)->AsSingle();
}
