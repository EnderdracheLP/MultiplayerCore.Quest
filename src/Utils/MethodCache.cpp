#include "Utils/MethodCache.hpp"
using namespace UnityEngine::Playables;
using namespace System::Linq;

namespace MultiplayerCore {
    void MethodCache::SetPlayableAsset(PlayableDirector* instance, PlayableAsset* asset) {
        static auto setPlayableAsset = il2cpp_utils::resolve_icall<void, Il2CppObject*, PlayableAsset*>("UnityEngine.Playables.PlayableDirector::SetPlayableAsset");
        setPlayableAsset(instance, asset);
    }

    const MethodInfo* MethodCache::get_Enumerable_ToList_Generic() {
        static auto* Enumerable_ToList_Generic = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(Enumerable*), "ToList", 1));
        return Enumerable_ToList_Generic;
    }

    const MethodInfo* MethodCache::get_Enumerable_Take_Generic() {
        static auto* Enumerable_Take_Generic = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(Enumerable*), "Take", 2));
        return Enumerable_Take_Generic;
    }

    const MethodInfo* MethodCache::get_Enumerable_ToArray_Generic() {
        static auto* Enumerable_ToArray_Generic = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(Enumerable*), "ToArray", 1));
        return Enumerable_ToArray_Generic;
    }
}