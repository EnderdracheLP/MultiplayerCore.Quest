#pragma once
#include "main.hpp"
#include "UnityEngine/Playables/PlayableAsset.hpp"
#include "UnityEngine/Playables/PlayableDirector.hpp"
#include "System/Linq/Enumerable.hpp"

namespace MultiplayerCore {
    struct MethodCache
    {
        static void SetPlayableAsset(UnityEngine::Playables::PlayableDirector* instance, UnityEngine::Playables::PlayableAsset* asset);

        static const MethodInfo* get_Enumerable_ToList_Generic();

        static const MethodInfo* get_Enumerable_Take_Generic();

        static const MethodInfo* get_Enumerable_ToArray_Generic();
    };
}