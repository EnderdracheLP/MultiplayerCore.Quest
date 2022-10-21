#include "Installers/AppInstaller.hpp"
#include "logging.hpp"
#include "UI/CenterScreenLoading.hpp"
#include "../shared/Networking/"

DEFINE_TYPE(MultiplayerCore::Installers, MpGameInstaller);

namespace MultiplayerCore::Installers {
    void MpMenuInstaller::InstallBindings() {
        ::Zenject::DiContainer* container = get_Container();
        INFO("Container: {}", container);

        /// TODO: get this working on Quest: Container.BindInterfacesAndSelfTo<MpColorsUI>().AsSingle();
        /// Container.BindInterfacesAndSelfTo<MpRequirementsUI>().AsSingle();
        /// Container.BindInterfacesAndSelfTo<MpLoadingIndicator>().AsSingle();

        /// Inject sira stuff that didn't get injected on appinit
        /// Container.Inject(Container.Resolve<NetworkPlayerEntitlementChecker>());

        // container->BindInterfacesAndSelfTo<MultiplayerCore::UI::MpColorUI*>()->AsSingle();
        // container->BindInterfacesAndSelfTo<MultiplayerCore::UI::MpRequirementsUI*>()->AsSingle();
        container->BindInterfacesAndSelfTo<MultiplayerCore::UI::CenterScreenLoading*>()->AsSingle();
        container->Inject(container->Resolve<>)
    };
}