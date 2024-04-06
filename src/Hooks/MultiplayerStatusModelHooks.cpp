#include "hooking.hpp"
#include "logging.hpp"

#include "GlobalNamespace/MultiplayerStatusData.hpp"
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
    // We return an object because that's what it'd return anyways, some type of object
    // First, we get our generic container:
    auto* genMethod = info->genericMethod;
    // Then, we look at our context, and get our method instantiation
    auto* method_inst = genMethod->context.method_inst;
    DEBUG("JsonConvert_DeserializeObject_MultiplayerStatusData");
    // Then, we compare our types
    if (method_inst->type_argc == 1 && il2cpp_functions::type_equals(method_inst->type_argv[0], il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_type<GlobalNamespace::MultiplayerStatusData*>::get())) {
        // THIS IS OUR CALL! LETS DO OUR CUSTOM BEHAVIOR HERE
        DEBUG("JsonConvert_DeserializeObject_MultiplayerStatusData CUSTOM TYPE RETURN");
        return MultiplayerCore::Models::MpStatusData::New_ctor(value);
    } else {
        // call orig here, remember to pass the info parameter to your orig call!
        return JsonConvert_DeserializeObject_MultiplayerStatusData(value, info);
    }
}

const MethodInfo* getJsonUtility_FromJson_MultiplayerStatusData_MethodInfo() {
    static const MethodInfo* ___internal__method = THROW_UNLESS((::il2cpp_utils::FindMethod("UnityEngine", "JsonUtility", "FromJson",
        std::vector<Il2CppClass*>{classof(GlobalNamespace::MultiplayerStatusData*)},
        ::std::vector<const Il2CppType*>{::il2cpp_utils::ExtractIndependentType<StringW>()})));
    static const MethodInfo* ___generic__method = THROW_UNLESS(::il2cpp_utils::MakeGenericMethod(___internal__method, std::vector<Il2CppClass*>{classof(GlobalNamespace::MultiplayerStatusData*)}));
    return ___generic__method;
}

MAKE_AUTO_HOOK_FIND_VERBOSE(JsonUtility_FromJson_MultiplayerStatusData, getJsonUtility_FromJson_MultiplayerStatusData_MethodInfo(), GlobalNamespace::MultiplayerStatusData*, StringW value, const MethodInfo* info) {
    // We return an object because that's what it'd return anyways, some type of object
    // First, we get our generic container:
    auto* genMethod = info->genericMethod;
    // Then, we look at our context, and get our method instantiation
    auto* method_inst = genMethod->context.method_inst;
    DEBUG("JsonUtility_FromJson_MultiplayerStatusData");
    // Then, we compare our types
    if (method_inst->type_argc == 1 && il2cpp_functions::type_equals(method_inst->type_argv[0], il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_type<GlobalNamespace::MultiplayerStatusData*>::get())) {
        // THIS IS OUR CALL! LETS DO OUR CUSTOM BEHAVIOR HERE
        DEBUG("JsonUtility_FromJson_MultiplayerStatusData CUSTOM TYPE RETURN");
        return MultiplayerCore::Models::MpStatusData::New_ctor(value);
    } else {
        // call orig here, remember to pass the info parameter to your orig call!
        return JsonUtility_FromJson_MultiplayerStatusData(value, info);
    }
}
