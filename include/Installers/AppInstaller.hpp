#pragma once

#include "custom-types/shared/macros.hpp"
#include "Zenject/Installer.hpp"

#include "Zenject/Installer.hpp"
#include "Zenject/ConcreteIdBinderGeneric_1.hpp"
#include "Zenject/DiContainer.hpp"
#include "Zenject/FromBinderNonGeneric.hpp"

#include "logging.hpp"

DECLARE_CLASS_CODEGEN(MultiplayerCore::Installers, AppInstaller, ::Zenject::Installer,
    DECLARE_DEFAULT_CTOR();
    DECLARE_OVERRIDE_METHOD(void, InstallBindings, il2cpp_utils::il2cpp_type_check::MetadataGetter<&::Zenject::Installer::InstallBindings>::get());
)