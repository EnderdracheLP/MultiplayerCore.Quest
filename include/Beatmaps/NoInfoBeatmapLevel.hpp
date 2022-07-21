#pragma once
#include "main.hpp"
#include "Beatmaps/Abstractions/MpBeatmapLevel.hpp"
#include "Beatmaps/Packets/MpBeatmapPacket.hpp"
#include "UnityEngine/AudioClip.hpp"
#include "System/Collections/Generic/IReadOnlyList_1.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"


DECLARE_CLASS_CUSTOM_DLL(MultiplayerCore::Beatmaps, NoInfoBeatmapLevel, Abstractions::MpBeatmapLevel, "MultiplayerCore.Beatmaps",

	DECLARE_CTOR(New, StringW hash);

	DECLARE_INSTANCE_FIELD(System::Threading::Tasks::Task_1<UnityEngine::Sprite*>*, coverImageTask);

	// Getter overrides
	DECLARE_OVERRIDE_METHOD(StringW, get_songName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songName>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songSubName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songSubName>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songAuthorName>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_levelAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_levelAuthorName>::get());

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