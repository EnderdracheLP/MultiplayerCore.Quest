#include "Installers/AppInstaller.hpp"

DEFINE_TYPE(MultiplayerCore::Installers, AppInstaller)

namespace MultiplayerCore::Installers {
    void AppInstaller::InstallBindings() {
        ::Zenject::DiContainer* container = get_Container();        
        DEBUG("AppContainer address: {}", fmt:ptr(container));
            // Container.BindInstance(new UBinder<Plugin, BeatSaver>(_beatsaver)).AsSingle();
            // Container.BindInterfacesAndSelfTo<MpPacketSerializer>().AsSingle();
            // Container.BindInterfacesAndSelfTo<MpPlayerManager>().AsSingle();
            // Container.Bind<MpLevelDownloader>().ToSelf().AsSingle();
            // Container.Bind<MpBeatmapLevelProvider>().ToSelf().AsSingle();
            // Container.BindInterfacesAndSelfTo<CustomLevelsPatcher>().AsSingle();
            // Container.BindInterfacesAndSelfTo<NetworkConfigPatcher>().AsSingle();
            // Container.BindInterfacesAndSelfTo<ModeSelectionPatcher>().AsSingle();
            // Container.BindInterfacesAndSelfTo<PlayerCountPatcher>().AsSingle();
            // Container.Bind<BGNetDebugLogger>().ToSelf().AsSingle();

        DEBUG("Bound all requested dependencies.");
    }
}