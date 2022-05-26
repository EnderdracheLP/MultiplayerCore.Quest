#pragma once
#include "System/Enum.hpp"
#include "System/ValueType.hpp"
#include <string>

namespace MultiplayerCore {
    class EnumUtils {
    public:
        template<typename T>
        static std::string GetEnumName(T enumValue) {
            return EnumUtils::GetEnumName<T>(enumValue, false);
        }

        template<typename T>
        static std::string GetEnumName(T enumValue, bool ignoreCase) {
            static_assert(std::is_convertible_v<T, System::Enum>);
            std::string enumName = "";
            try {
                Il2CppClass* enumClass = classof(T);
                Il2CppReflectionType* enumType = enumClass ? ::il2cpp_utils::GetSystemType(enumClass) : nullptr;
                Il2CppObject* boxedValue = il2cpp_functions::value_box(classof(T), &enumValue.value);
                if (enumType != nullptr && System::Enum::IsDefined(enumType, boxedValue)) {
                    enumName = static_cast<std::string>(System::Enum::GetName(enumType, boxedValue));
                }
                else {
                    enumName = "";
                }
                if (ignoreCase) {
                    std::transform(enumName.begin(), enumName.end(), enumName.begin(), ::tolower);
                }
            }
            catch (il2cpp_utils::RunMethodException const& e) {
                getLogger().error("REPORT TO ENDER: %s", e.what());
                e.log_backtrace();
            }
            catch (const std::runtime_error& e) {
                getLogger().error("REPORT TO ENDER: %s", e.what());
            }
            catch (...) {
                getLogger().error("REPORT TO ENDER: Unknown exception");
            }
            return enumName;
        }
    };
}