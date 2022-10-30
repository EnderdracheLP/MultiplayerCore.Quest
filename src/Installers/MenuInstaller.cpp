#include "Installers/MenuInstaller.hpp"



DEFINE_TYPE(MultiplayerCore::Installers, MenuInstaller)

namespace MultiplayerCore::Installers {
    void MenuInstaller::InstallBindings() {
        ::Zenject::DiContainer* container = get_Container();        
        DEBUG("MenuContainer address: {}", fmt:ptr(container));

            // Container.BindInterfacesAndSelfTo<MpColorsUI>().AsSingle();
            // Container.BindInterfacesAndSelfTo<MpRequirementsUI>().AsSingle();
            // Container.BindInterfacesAndSelfTo<MpLoadingIndicator>().AsSingle();

            // // Inject sira stuff that didn't get injected on appinit
            // Container.Inject(Container.Resolve<NetworkPlayerEntitlementChecker>());

        DEBUG("Bound all requested dependencies.");
    }
}