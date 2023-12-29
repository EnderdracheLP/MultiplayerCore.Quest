#pragma once

#include "System/Enum.hpp"
#include <string>

namespace MultiplayerCore::Utils {
    struct EnumUtils {
        template<typename T>
        static T GetEnumValue(const std::string& enumName) {
            return EnumUtils::GetEnumValue<T>(enumName, false);
        }

        template<typename T>
        static T GetEnumValue(const std::string& enumName, bool ignoreCase) {
            StringW value = enumName;
            T enumResult = T(0);
            try {
                static auto* ___internal__method = THROW_UNLESS((::il2cpp_utils::FindMethod("System", "Enum", "TryParse", std::vector<Il2CppClass*>{::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<T>::get()}, ::std::vector<const Il2CppType*>{::il2cpp_utils::ExtractType(value), ::il2cpp_utils::ExtractType(ignoreCase), ::il2cpp_utils::ExtractIndependentType<T&>()})));
                static auto* ___generic__method = THROW_UNLESS(::il2cpp_utils::MakeGenericMethod(___internal__method, std::vector<Il2CppClass*>{::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<T>::get()}));
                ::il2cpp_utils::RunMethodRethrow<bool, false>(static_cast<System::Object*>(nullptr), ___generic__method, value, ignoreCase, byref(enumResult));

                // Il2CppClass* enumClass = classof(T);
                // Il2CppReflectionType* enumType = enumClass ? ::il2cpp_utils::GetSystemType(enumClass) : nullptr;
                // if (enumType != nullptr) {
                //     // static bool TryParseEnum(::System::Type* enumType, ::StringW value, bool ignoreCase, ByRef<::System::Enum::EnumResult> parseResult);
                //     System::Enum::EnumResult parseResult;
                //     if (System::Enum::TryParseEnum(enumType, enumName, ignoreCase, byref(parseResult))) {
                //         if (parseResult.parsedEnum) enumValue = *reinterpret_cast<T*>(il2cpp_functions::object_unbox(parseResult.parsedEnum));
                //     }
                //     // enumValue = static_cast<T>(System::Enum::Parse(enumType, enumName));
                // }
            } catch (il2cpp_utils::exceptions::StackTraceException const& e) {
                // TODO: logging
                // getMpCoreLogger().error("REPORT TO ENDER StackTraceException: %s", e.what());
                // e.log_backtrace();
            } catch (il2cpp_utils::RunMethodException const& e) {
                // getMpCoreLogger().error("REPORT TO ENDER RunMethodException: %s", e.what());
                // e.log_backtrace();
            } catch (const std::runtime_error& e) {
                // getMpCoreLogger().error("REPORT TO ENDER runtime_error: %s", e.what());
            } catch (...) {
                // getMpCoreLogger().error("REPORT TO ENDER: Unknown exception");
            }
            return enumResult;
        }
    };
}
