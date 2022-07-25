#pragma once
#include "custom-types/shared/macros.hpp"

#include "GlobalNamespace/MultiplayerStatusData.hpp"

// //forward declarations
// namespace MultiplayerCore::Models {
//     struct MpStatusData::RequiredMod;
// }

DECLARE_CLASS_CODEGEN_DLL(MultiplayerCore::Models, MpStatusData, GlobalNamespace::MultiplayerStatusData, "MultiplayerCore.Models",
    DECLARE_CTOR(New, StringW);
    
    public:
        std::vector<ModInfo> requiredMods;
)

// namespace MultiplayerCore::Models {
//     struct MpStatusData::RequiredMod {
//         StringW id;
//         StringW version;
//     };
// }