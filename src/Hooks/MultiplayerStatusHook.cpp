#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "Models/MpStatusData.hpp"
#include "Newtonsoft/Json/JsonConvert.hpp"
#include "GlobalNamespace/MultiplayerStatusData.hpp"
using namespace GlobalNamespace;
using namespace MultiplayerCore::Models;

namespace MultiplayerCore {

    const MethodInfo* getMultiplayerStatusData_GetStatus_MethodInfo() {
        static const MethodInfo* ___internal__method = THROW_UNLESS((::il2cpp_utils::FindMethod("Newtonsoft.Json", "JsonConvert", "DeserializeObject", 
            std::vector<Il2CppClass*>{classof(MultiplayerStatusData*)}, 
            ::std::vector<const Il2CppType*>{::il2cpp_utils::ExtractIndependentType<StringW>()})));
        static const MethodInfo* ___generic__method = THROW_UNLESS(::il2cpp_utils::MakeGenericMethod(___internal__method, std::vector<Il2CppClass*>{classof(MultiplayerStatusData*)}));
        return ___generic__method;
    }


    MAKE_HOOK_FIND_VERBOSE(JsonConvert_DeserializeObject_MultiplayerStatusData, getMultiplayerStatusData_GetStatus_MethodInfo(), MultiplayerStatusData*, StringW value, const MethodInfo* info)
    {
        getLogger().debug("[JsonConvert_DeserializeObject_MultiplayerStatusData] Called, value string '%s'", static_cast<std::string>(value).c_str());
        // We return an object because that's what it'd return anyways, some type of object
        // First, we get our generic container:
        auto* genMethod = info->genericMethod;
        // Then, we look at our context, and get our method instantiation
        auto* method_inst = genMethod->context.method_inst;
        // Then, we compare our types
        if (method_inst->type_argc == 1 && il2cpp_functions::type_equals(method_inst->type_argv[0], il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_type<MultiplayerStatusData*>::get())) {
            // THIS IS OUR CALL! LETS DO OUR CUSTOM BEHAVIOR HERE
            getLogger().debug("Our call");
            return MpStatusData::New_ctor(value);
        } else {
            getLogger().debug("Not our call");
            // call orig here, remember to pass the info parameter to your orig call!
            return JsonConvert_DeserializeObject_MultiplayerStatusData(value, info);
        }
    }
    
    void Hooks::MultiplayerStatusHook()
    {
        INSTALL_HOOK(getLogger(), JsonConvert_DeserializeObject_MultiplayerStatusData);
    }
}