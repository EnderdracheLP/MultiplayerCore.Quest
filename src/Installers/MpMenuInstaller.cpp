#include "Installers/MpMenuInstaller.hpp"

#include "UI/MpColorsUI.hpp"
#include "UI/MpLoadingIndicator.hpp"
#include "UI/MpRequirementsUI.hpp"

#include "Zenject/DiContainer.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"

DEFINE_TYPE(MultiplayerCore::Installers, MpMenuInstaller);

using namespace MultiplayerCore::UI;

namespace MultiplayerCore::Installers {
    void MpMenuInstaller::InstallBindings() {
        auto container = get_Container();

        container->BindInterfacesAndSelfTo<MpColorsUI*>()->AsSingle();
        container->BindInterfacesAndSelfTo<MpRequirementsUI*>()->AsSingle();
        container->BindInterfacesAndSelfTo<MpLoadingIndicator*>()->AsSingle();
    }
}
