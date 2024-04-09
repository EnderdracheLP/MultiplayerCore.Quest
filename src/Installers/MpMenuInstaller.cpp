#include "Installers/MpMenuInstaller.hpp"

#include "UI/MpColorsUI.hpp"
#include "UI/MpLoadingIndicator.hpp"
#include "UI/MpRequirementsUI.hpp"
#include "Patchers/BeatmapSelectionViewPatcher.hpp"

#include "Zenject/DiContainer.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"

DEFINE_TYPE(MultiplayerCore::Installers, MpMenuInstaller);

using namespace MultiplayerCore::UI;
using namespace MultiplayerCore::Patchers;

namespace MultiplayerCore::Installers {
    void MpMenuInstaller::InstallBindings() {
        auto container = get_Container();

        container->BindInterfacesAndSelfTo<MpColorsUI*>()->AsSingle();
        container->BindInterfacesAndSelfTo<MpRequirementsUI*>()->AsSingle();
        container->BindInterfacesAndSelfTo<MpRequirementsUI*>()->AsSingle();
        container->BindInterfacesAndSelfTo<MpLoadingIndicator*>()->AsSingle();
        container->BindInterfacesAndSelfTo<BeatmapSelectionViewPatcher*>()->AsSingle();
    }
}
