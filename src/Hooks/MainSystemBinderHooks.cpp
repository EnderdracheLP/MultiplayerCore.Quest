#include "logging.hpp"
#include "hooking.hpp"

#include "lapiz/shared/arrayutils.hpp"
#include "GlobalNamespace/MainSystemInit.hpp"
#include "GlobalNamespace/NodePoseSyncStateManager.hpp"
#include "GlobalNamespace/INodePoseSyncStateManager.hpp"
#include "GlobalNamespace/ScoreSyncStateManager.hpp"
#include "GlobalNamespace/IScoreSyncStateManager.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/ConcreteIdBinderGeneric_1.hpp"
#include "Zenject/ConcreteIdBinderNonGeneric.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"
#include "Objects/MpEntitlementChecker.hpp"
#include "NodePoseSyncState/MpNodePoseSyncStateManager.hpp"
#include "ScoreSyncState/MpScoreSyncStateManager.hpp"

MAKE_AUTO_HOOK_ORIG_MATCH(MainSystemInit_InstallBindings, &::GlobalNamespace::MainSystemInit::InstallBindings, void, GlobalNamespace::MainSystemInit* self, Zenject::DiContainer* container, bool isRunningFromTests) {
    MainSystemInit_InstallBindings(self, container, isRunningFromTests);

    container->Unbind<GlobalNamespace::NetworkPlayerEntitlementChecker*>();
    auto bindarray = Lapiz::ArrayUtils::TypeArray<GlobalNamespace::NetworkPlayerEntitlementChecker*>();
    auto toarray = Lapiz::ArrayUtils::TypeArray<MultiplayerCore::Objects::MpEntitlementChecker*>();
    container->Bind(reinterpret_cast<::System::Collections::Generic::IEnumerable_1<System::Type*>*>(bindarray.convert()))->To(reinterpret_cast<::System::Collections::Generic::IEnumerable_1<System::Type*>*>(toarray.convert()))->FromNewComponentOnRoot()->AsSingle()->NonLazy();

    container->Unbind<GlobalNamespace::NodePoseSyncStateManager*>();
    container->Unbind<GlobalNamespace::INodePoseSyncStateManager*>();

    bindarray = Lapiz::ArrayUtils::TypeArray<MultiplayerCore::NodePoseSyncState::MpNodePoseSyncStateManager*, GlobalNamespace::NodePoseSyncStateManager*, GlobalNamespace::INodePoseSyncStateManager*, Zenject::IInitializable*, System::IDisposable*>();
    toarray = Lapiz::ArrayUtils::TypeArray<MultiplayerCore::NodePoseSyncState::MpNodePoseSyncStateManager*>();
    container->Bind(reinterpret_cast<::System::Collections::Generic::IEnumerable_1<System::Type*>*>(bindarray.convert()))->To(reinterpret_cast<::System::Collections::Generic::IEnumerable_1<System::Type*>*>(toarray.convert()))->FromNewComponentOnRoot()->AsSingle();

    container->Unbind<GlobalNamespace::ScoreSyncStateManager*>();
    container->Unbind<GlobalNamespace::IScoreSyncStateManager*>();

    bindarray = Lapiz::ArrayUtils::TypeArray<MultiplayerCore::ScoreSyncState::MpScoreSyncStateManager*, GlobalNamespace::ScoreSyncStateManager*, GlobalNamespace::IScoreSyncStateManager*, Zenject::IInitializable*, System::IDisposable*>();
    toarray = Lapiz::ArrayUtils::TypeArray<MultiplayerCore::ScoreSyncState::MpScoreSyncStateManager*>();
    container->Bind(reinterpret_cast<::System::Collections::Generic::IEnumerable_1<System::Type*>*>(bindarray.convert()))->To(reinterpret_cast<::System::Collections::Generic::IEnumerable_1<System::Type*>*>(toarray.convert()))->FromNewComponentOnRoot()->AsSingle();
}
