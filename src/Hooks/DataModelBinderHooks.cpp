#include "hooking.hpp"
#include "logging.hpp"

#include "Objects/MpPlayersDataModel.hpp"

#include "Zenject/DiContainer.hpp"
#include "Zenject/ConcreteIdBinderNonGeneric.hpp"
#include "GlobalNamespace/LobbyDataModelInstaller.hpp"

#include "lapiz/shared/arrayutils.hpp"

MAKE_AUTO_HOOK_MATCH(LobbyDataModelInstaller_InstallBindings, &::GlobalNamespace::LobbyDataModelInstaller::InstallBindings, void, GlobalNamespace::LobbyDataModelInstaller* self) {
    LobbyDataModelInstaller_InstallBindings(self);
    auto container = self->get_Container();
    container->Unbind<GlobalNamespace::LobbyPlayersDataModel*>();
    auto bindarray = Lapiz::ArrayUtils::TypeArray<GlobalNamespace::ILobbyPlayersDataModel*, GlobalNamespace::LobbyPlayersDataModel*, MultiplayerCore::Objects::MpPlayersDataModel*, System::IDisposable*>();
    auto toarray = Lapiz::ArrayUtils::TypeArray<MultiplayerCore::Objects::MpPlayersDataModel*>();
    container->Bind(bindarray)->To(toarray)->AsSingle();
}
