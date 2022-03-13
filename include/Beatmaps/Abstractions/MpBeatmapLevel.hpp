#pragma once
#include "main.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/PreviewDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/EnvironmentInfoSO.hpp"

#include "UnityEngine/Sprite.hpp"

#include "System/Threading/Tasks/Task_1.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES_DLL(MultiplayerCore::Beatmaps::Abstractions, MpBeatmapLevel,
    Il2CppObject,
    { classof(GlobalNamespace::IPreviewBeatmapLevel*) },
    "MultiplayerCore.Beatmaps.Abstractions",

    DECLARE_CTOR(New);

    DECLARE_INSTANCE_METHOD(StringW, get_levelHash);

    DECLARE_OVERRIDE_METHOD(StringW, get_levelID, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_levelID>::get());

    DECLARE_OVERRIDE_METHOD(StringW, get_songName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songName>::get());
    DECLARE_OVERRIDE_METHOD(StringW, get_songSubName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songSubName>::get());
    DECLARE_OVERRIDE_METHOD(StringW, get_songAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songAuthorName>::get());
    DECLARE_OVERRIDE_METHOD(StringW, get_levelAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_levelAuthorName>::get());

    DECLARE_OVERRIDE_METHOD(float, get_beatsPerMinute, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_beatsPerMinute>::get());
    DECLARE_OVERRIDE_METHOD(float, get_songDuration, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songDuration>::get());
    DECLARE_OVERRIDE_METHOD(float, get_previewStartTime, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_previewStartTime>::get());
    DECLARE_OVERRIDE_METHOD(float, get_previewDuration, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_previewDuration>::get());
    DECLARE_OVERRIDE_METHOD(ArrayW<GlobalNamespace::PreviewDifficultyBeatmapSet*>, get_previewDifficultyBeatmapSets, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_previewDifficultyBeatmapSets>::get());

    DECLARE_OVERRIDE_METHOD(float, get_songTimeOffset, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songTimeOffset>::get());
    DECLARE_OVERRIDE_METHOD(float, get_shuffle, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_shuffle>::get());
    DECLARE_OVERRIDE_METHOD(float, get_shufflePeriod, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_shufflePeriod>::get());
    DECLARE_OVERRIDE_METHOD(GlobalNamespace::EnvironmentInfoSO*, get_environmentInfo, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_environmentInfo>::get());
    DECLARE_OVERRIDE_METHOD(GlobalNamespace::EnvironmentInfoSO*, get_allDirectionsEnvironmentInfo, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_allDirectionsEnvironmentInfo>::get());

    DECLARE_OVERRIDE_METHOD(System::Threading::Tasks::Task_1<UnityEngine::Sprite*>*, GetCoverImageAsync, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::GetCoverImageAsync>::get(), System::Threading::CancellationToken cancellationToken);
)

#include "beatsaber-hook/shared/utils/il2cpp-utils-methods.hpp"
template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<::StringW(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::*)()>(&MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::get_levelHash)> {
    static const MethodInfo* get() {
        return ::il2cpp_utils::FindMethod(classof(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel*), "get_levelHash", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{});
    }
};
template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<::StringW(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::*)()>(&MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::get_levelID)> {
    static const MethodInfo* get() {
        return ::il2cpp_utils::FindMethod(classof(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel*), "get_levelID", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{});
    }
};
template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<::StringW(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::*)()>(&MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::get_songName)> {
    static const MethodInfo* get() {
        return ::il2cpp_utils::FindMethod(classof(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel*), "get_songName", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{});
    }
};
template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<::StringW(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::*)()>(&MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::get_songSubName)> {
    static const MethodInfo* get() {
        return ::il2cpp_utils::FindMethod(classof(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel*), "get_songSubName", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{});
    }
};
template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<::StringW(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::*)()>(&MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::get_songAuthorName)> {
    static const MethodInfo* get() {
        return ::il2cpp_utils::FindMethod(classof(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel*), "get_songAuthorName", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{});
    }
};
template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<::StringW(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::*)()>(&MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::get_levelAuthorName)> {
    static const MethodInfo* get() {
        return ::il2cpp_utils::FindMethod(classof(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel*), "get_levelAuthorName", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{});
    }
};
template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<float (MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::*)()>(&MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::get_beatsPerMinute)> {
    static const MethodInfo* get() {
        return ::il2cpp_utils::FindMethod(classof(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel*), "get_beatsPerMinute", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{});
    }
};
template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<float (MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::*)()>(&MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::get_songTimeOffset)> {
    static const MethodInfo* get() {
        return ::il2cpp_utils::FindMethod(classof(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel*), "get_songTimeOffset", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{});
    }
};
template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<float (MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::*)()>(&MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::get_shuffle)> {
    static const MethodInfo* get() {
        return ::il2cpp_utils::FindMethod(classof(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel*), "get_shuffle", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{});
    }
};
template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<float (MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::*)()>(&MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::get_shufflePeriod)> {
    static const MethodInfo* get() {
        return ::il2cpp_utils::FindMethod(classof(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel*), "get_shufflePeriod", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{});
    }
};
template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<float (MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::*)()>(&MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::get_previewStartTime)> {
    static const MethodInfo* get() {
        return ::il2cpp_utils::FindMethod(classof(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel*), "get_previewStartTime", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{});
    }
};
template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<float (MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::*)()>(&MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::get_previewDuration)> {
    static const MethodInfo* get() {
        return ::il2cpp_utils::FindMethod(classof(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel*), "get_previewDuration", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{});
    }
};
template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<float (MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::*)()>(&MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::get_songDuration)> {
    static const MethodInfo* get() {
        return ::il2cpp_utils::FindMethod(classof(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel*), "get_songDuration", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{});
    }
};
template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<GlobalNamespace::EnvironmentInfoSO* (MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::*)()>(&MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::get_environmentInfo)> {
    static const MethodInfo* get() {
        return ::il2cpp_utils::FindMethod(classof(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel*), "get_environmentInfo", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{});
    }
};
template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<GlobalNamespace::EnvironmentInfoSO* (MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::*)()>(&MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::get_allDirectionsEnvironmentInfo)> {
    static const MethodInfo* get() {
        return ::il2cpp_utils::FindMethod(classof(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel*), "get_allDirectionsEnvironmentInfo", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{});
    }
};
template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<::ArrayW<GlobalNamespace::PreviewDifficultyBeatmapSet*>(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::*)()>(&MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::get_previewDifficultyBeatmapSets)> {
    static const MethodInfo* get() {
        return ::il2cpp_utils::FindMethod(classof(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel*), "get_previewDifficultyBeatmapSets", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{});
    }
};
template<>
struct ::il2cpp_utils::il2cpp_type_check::MetadataGetter<static_cast<System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* (MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::*)(System::Threading::CancellationToken)>(&MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel::GetCoverImageAsync)> {
    static const MethodInfo* get() {
        static auto* cancellationToken = &::il2cpp_utils::GetClassFromName("System.Threading", "CancellationToken")->byval_arg;
        return ::il2cpp_utils::FindMethod(classof(MultiplayerCore::Beatmaps::Abstractions::MpBeatmapLevel*), "GetCoverImageAsync", std::vector<Il2CppClass*>(), ::std::vector<const Il2CppType*>{cancellationToken});
    }
};
