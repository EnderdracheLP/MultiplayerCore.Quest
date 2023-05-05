#pragma once

#include "custom-types/shared/macros.hpp"
#include "GlobalNamespace/MultiplayerStatusData.hpp"

DECLARE_CLASS_CODEGEN(MultiplayerCore::Models, MpStatusData, GlobalNamespace::MultiplayerStatusData,
    DECLARE_CTOR(New, StringW json);
    DECLARE_INSTANCE_FIELD(StringW, maximumAppVersion);
    public:
        std::vector<ModInfo> requiredMods;
)
