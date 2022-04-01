#pragma once
#include "main.hpp"
#include "Beatmaps/Abstractions/MpBeatmapLevel.hpp"
#include "Beatmaps/Packets/MpBeatmapPacket.hpp"
#include "UnityEngine/AudioClip.hpp"
#include "System/Collections/Generic/IReadOnlyList_1.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

//DECLARE_CLASS_CODEGEN_INTERFACES_DLL(MultiplayerCore::Beatmaps, NetworkBeatmapLevel,
//Il2CppObject,
//{ classof(GlobalNamespace::IPreviewBeatmapLevel*) },
//"MultiplayerCore.Beatmaps",
DECLARE_CLASS_CUSTOM_DLL(MultiplayerCore::Beatmaps, NoInfoBeatmapLevel, Abstractions::MpBeatmapLevel, "MultiplayerCore.Beatmaps",
//___DECLARE_TYPE_WRAPPER_INHERITANCE(MultiplayerCore::Beatmaps, NetworkBeatmapLevel, Il2CppTypeEnum::IL2CPP_TYPE_CLASS,
//	Il2CppObject, "MultiplayerCore.Beatmaps",
//	{ classof(MpBeatmapPacket) }, 0, nullptr,
	DECLARE_CTOR(New, StringW hash);

	// Level Hash
	DECLARE_INSTANCE_FIELD(StringW, levelHash);

	//DECLARE_INSTANCE_FIELD(StringW, levelHash);

	//DECLARE_INSTANCE_FIELD(StringW, songName);
	//DECLARE_INSTANCE_FIELD(StringW, songSubName);
	//DECLARE_INSTANCE_FIELD(StringW, songAuthorName);
	//DECLARE_INSTANCE_FIELD(StringW, levelAuthorName);
	//DECLARE_INSTANCE_FIELD(float, beatsPerMinute);
	//DECLARE_INSTANCE_FIELD(float, songDuration);

	DECLARE_INSTANCE_FIELD(System::Threading::Tasks::Task_1<UnityEngine::Sprite*>*, coverImageTask);

	// Getter overrides
	DECLARE_INSTANCE_METHOD(StringW, get_levelHash);

	DECLARE_OVERRIDE_METHOD(StringW, get_levelID, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_levelID>::get());

	DECLARE_OVERRIDE_METHOD(StringW, get_songName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songName>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songSubName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songSubName>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songAuthorName>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_levelAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_levelAuthorName>::get());

	//DECLARE_OVERRIDE_METHOD(float, get_beatsPerMinute, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_beatsPerMinute>::get());
	//DECLARE_OVERRIDE_METHOD(float, get_songDuration, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songDuration>::get());
	//DECLARE_OVERRIDE_METHOD(float, get_previewStartTime, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_previewStartTime>::get());
	//DECLARE_OVERRIDE_METHOD(float, get_previewDuration, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_previewDuration>::get());
	//DECLARE_OVERRIDE_METHOD(::System::Collections::Generic::IReadOnlyList_1<::GlobalNamespace::PreviewDifficultyBeatmapSet*>*, get_previewDifficultyBeatmapSets, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_previewDifficultyBeatmapSets>::get());

	//DECLARE_OVERRIDE_METHOD(float, get_songTimeOffset, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songTimeOffset>::get());
	//DECLARE_OVERRIDE_METHOD(float, get_shuffle, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_shuffle>::get());
	//DECLARE_OVERRIDE_METHOD(float, get_shufflePeriod, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_shufflePeriod>::get());
	//DECLARE_OVERRIDE_METHOD(GlobalNamespace::EnvironmentInfoSO*, get_environmentInfo, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_environmentInfo>::get());
	//DECLARE_OVERRIDE_METHOD(GlobalNamespace::EnvironmentInfoSO*, get_allDirectionsEnvironmentInfo, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_allDirectionsEnvironmentInfo>::get());

	DECLARE_OVERRIDE_METHOD(System::Threading::Tasks::Task_1<UnityEngine::Sprite*>*, GetCoverImageAsync, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::GetCoverImageAsync>::get(), System::Threading::CancellationToken cancellationToken);

	public:
		// Conversion operators to BaseType MpBeatmapLevel
		operator Abstractions::MpBeatmapLevel* () noexcept {
			return reinterpret_cast<Abstractions::MpBeatmapLevel*>(this);
		}

		// Conversion operators to IPreviewBeatmapLevel
		operator GlobalNamespace::IPreviewBeatmapLevel* () noexcept {
			return reinterpret_cast<GlobalNamespace::IPreviewBeatmapLevel*>(this);
		}

		// Converter
		GlobalNamespace::IPreviewBeatmapLevel* get_preview() {
			return reinterpret_cast<GlobalNamespace::IPreviewBeatmapLevel*>(this);
		}
)