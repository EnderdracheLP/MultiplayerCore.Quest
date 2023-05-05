#include "hooking.hpp"
#include "logging.hpp"

#include "Models/MpStatusData.hpp"
#include "Newtonsoft/Json/JsonConvert.hpp"

const MethodInfo* getMultiplayerStatusData_GetStatus_MethodInfo() {
    static const MethodInfo* ___internal__method = THROW_UNLESS((::il2cpp_utils::FindMethod("Newtonsoft.Json", "JsonConvert", "DeserializeObject",
        std::vector<Il2CppClass*>{classof(GlobalNamespace::MultiplayerStatusData*)},
        ::std::vector<const Il2CppType*>{::il2cpp_utils::ExtractIndependentType<StringW>()})));
    static const MethodInfo* ___generic__method = THROW_UNLESS(::il2cpp_utils::MakeGenericMethod(___internal__method, std::vector<Il2CppClass*>{classof(GlobalNamespace::MultiplayerStatusData*)}));
    return ___generic__method;
}

// possibly does not call orig
MAKE_AUTO_HOOK_FIND_VERBOSE(JsonConvert_DeserializeObject_MultiplayerStatusData, getMultiplayerStatusData_GetStatus_MethodInfo(), GlobalNamespace::MultiplayerStatusData*, StringW value, const MethodInfo* info) {
    DEBUG("[JsonConvert_DeserializeObject_MultiplayerStatusData] Called, value string '{}'", value);
    DEBUG("[JsonConvert_DeserializeObject_MultiplayerStatusData] Checking pointers String value:'{}', MethodInfo info:'{}'", fmt::ptr(value.convert()), fmt::ptr(info));

    // We return an object because that's what it'd return anyways, some type of object
    // First, we get our generic container:
    auto* genMethod = info->genericMethod;
    // Then, we look at our context, and get our method instantiation
    auto* method_inst = genMethod->context.method_inst;
    DEBUG("[JsonConvert_DeserializeObject_MultiplayerStatusData] Checking genericMethod pointers Il2CppGenericMethod info->genericMethod:'{}', Il2CppGenericInst genericMethod->context.method_inst:'{}'", fmt::ptr(genMethod), fmt::ptr(method_inst));
    // Then, we compare our types
    if (method_inst->type_argc == 1 && il2cpp_functions::type_equals(method_inst->type_argv[0], il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_type<GlobalNamespace::MultiplayerStatusData*>::get())) {
        // THIS IS OUR CALL! LETS DO OUR CUSTOM BEHAVIOR HERE
        DEBUG("Our call");
        return MultiplayerCore::Models::MpStatusData::New_ctor(value);
    } else {
        DEBUG("Not our call, calling orig");
        // call orig here, remember to pass the info parameter to your orig call!
        return JsonConvert_DeserializeObject_MultiplayerStatusData(value, info);
    }
}
