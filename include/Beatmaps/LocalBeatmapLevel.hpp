#pragma once
#include "main.hpp"
#include "Beatmaps/Abstractions/MpBeatmapLevel.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CUSTOM_DLL(MultiplayerCore::Beatmaps, LocalBeatmapLevel, Abstractions::MpBeatmapLevel, "MultiplayerCore.Beatmaps",
//___DECLARE_TYPE_WRAPPER_INHERITANCE(MultiplayerCore::Beatmaps, NetworkBeatmapLevel, Il2CppTypeEnum::IL2CPP_TYPE_CLASS,
//	Il2CppObject, "MultiplayerCore.Beatmaps",
//	{ classof(MpBeatmapPacket) }, 0, nullptr,
	DECLARE_CTOR(New, StringW hash, GlobalNamespace::IPreviewBeatmapLevel* preview);

	// IPreviewBeatmapLevel
	DECLARE_INSTANCE_FIELD(GlobalNamespace::IPreviewBeatmapLevel*, _preview);
	DECLARE_INSTANCE_FIELD(StringW, levelHash);

	// Getter overrides
	DECLARE_OVERRIDE_METHOD(StringW, get_levelHash, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());

	DECLARE_OVERRIDE_METHOD(StringW, get_levelID, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelID>::get());

	DECLARE_OVERRIDE_METHOD(StringW, get_songName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songSubName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_levelAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(float, get_beatsPerMinute, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(float, get_songDuration, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(::System::Collections::Generic::IReadOnlyList_1<::GlobalNamespace::PreviewDifficultyBeatmapSet*>*, get_previewDifficultyBeatmapSets, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_previewDifficultyBeatmapSets>::get());


	public:
		template <class TPreview>
		static LocalBeatmapLevel* CS_Ctor(StringW hash, TPreview* preview) {
			static_assert(std::is_convertible_v<std::remove_pointer_t<TPreview>, GlobalNamespace::IPreviewBeatmapLevel>, "Make sure your Type Implements and is Convertible to IPreviewBeatmapLevel*");
			return LocalBeatmapLevel::New_ctor(hash, reinterpret_cast<GlobalNamespace::IPreviewBeatmapLevel*>(preview));
		}

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