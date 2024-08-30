#include "hooking.hpp"
#include "logging.hpp"
#include "Patchers/ModeSelectionPatcher.hpp"

#include "GlobalNamespace/MultiplayerStatusModel.hpp"
#include "GlobalNamespace/QuickPlaySetupModel.hpp"
#include "GlobalNamespace/MultiplayerUnavailableReasonMethods.hpp"

using namespace MultiplayerCore::Patchers;

// possibly does not call orig
MAKE_AUTO_HOOK_ORIG_MATCH(MultiplayerStatusModel_IsAvailabilityTaskValid, &::GlobalNamespace::MultiplayerStatusModel::IsAvailabilityTaskValid, bool, GlobalNamespace::MultiplayerStatusModel* self) {
    auto patcher = ModeSelectionPatcher::get_instance();
    if (patcher && patcher->UpdateStatusUrl())
        return false;

    return MultiplayerStatusModel_IsAvailabilityTaskValid(self);
}

// possibly does not call orig
MAKE_AUTO_HOOK_ORIG_MATCH(QuickPlaySetupModel_IsQuickPlaySetupTaskValid_2nd, &::GlobalNamespace::QuickPlaySetupModel::IsQuickPlaySetupTaskValid, bool, GlobalNamespace::QuickPlaySetupModel* self) {
    auto patcher = ModeSelectionPatcher::get_instance();
    if (patcher && patcher->UpdateStatusUrl())
        return false;

    return QuickPlaySetupModel_IsQuickPlaySetupTaskValid_2nd(self);
}

// possibly does not call orig
MAKE_AUTO_HOOK_ORIG_MATCH(MultiplayerUnavailableReasonMethods_TryGetMultiplayerUnavailableReason_2nd, &::GlobalNamespace::MultiplayerUnavailableReasonMethods::TryGetMultiplayerUnavailableReason, bool, GlobalNamespace::MultiplayerStatusData* data, ByRef<GlobalNamespace::MultiplayerUnavailableReason> reason) {
    if (data) return MultiplayerUnavailableReasonMethods_TryGetMultiplayerUnavailableReason_2nd(data, reason);

    return false;
}
