#pragma once

#include "custom-types/shared/macros.hpp"
#include "Zenject/Installer.hpp"
#include "Zenject/DiContainer.hpp"

DECLARE_CLASS_CODEGEN(MultiplayerCore::Installers, MpAppInstaller, ::Zenject::Installer,
    DECLARE_OVERRIDE_METHOD(void, InstallBindings, il2cpp_utils::il2cpp_type_check:::MetadataGetter<&::Zenject::InstallerBase::InstallBindings>::get());
    DECLARE_DEFAULT_CTOR();
)