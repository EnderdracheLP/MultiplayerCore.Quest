#include "Installers/MpAppInstaller.hpp"

#include "Networking/MpPacketSerializer.hpp"
#include "Players/MpPlayerManager.hpp"
#include "Objects/MpLevelDownloader.hpp"
#include "Objects/BGNetDebugLogger.hpp"
#include "Beatmaps/Providers/MpBeatmapLevelProvider.hpp"
#include "Patchers/ModeSelectionPatcher.hpp"
#include "Patchers/PlayerCountPatcher.hpp"
#include "Repositories/MpStatusRepository.hpp"

#include "Zenject/DiContainer.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"
#include "Zenject/ConcreteIdBinderGeneric_1.hpp"

DEFINE_TYPE(MultiplayerCore::Installers, MpAppInstaller);

using namespace MultiplayerCore::Networking;
using namespace MultiplayerCore::Players;
using namespace MultiplayerCore::Objects;
using namespace MultiplayerCore::Patchers;
using namespace MultiplayerCore::Beatmaps::Providers;
using namespace MultiplayerCore::Repositories;

namespace MultiplayerCore::Installers {
    void MpAppInstaller::InstallBindings() {
        auto container = get_Container();
        // networking stuff
        container->BindInterfacesAndSelfTo<MpPacketSerializer*>()->AsSingle();

        // player stuff
        container->BindInterfacesAndSelfTo<MpPlayerManager*>()->AsSingle();

        // beatmap stuff
        container->Bind<MpLevelDownloader*>()->ToSelf()->AsSingle();
        container->Bind<MpBeatmapLevelProvider*>()->ToSelf()->AsSingle();

        // patching stuff, will probably be done using hooks instead
        // container->BindInterfacesAndSelfTo<CustomLevelsPatcher*>()->AsSingle();
        // container->BindInterfacesAndSelfTo<NetworkConfigPatcher*>()->AsSingle();
        container->BindInterfacesAndSelfTo<ModeSelectionPatcher*>()->AsSingle();
        container->BindInterfacesAndSelfTo<PlayerCountPatcher*>()->AsSingle();

        // logging stuff
        container->Bind<BGNetDebugLogger*>()->ToSelf()->AsSingle();

        // repositories
        container->BindInterfacesAndSelfTo<MpStatusRepository*>()->AsSingle();
    }
}
