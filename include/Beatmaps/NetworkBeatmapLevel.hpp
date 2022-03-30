#pragma once
#include "main.hpp"
#include "Beatmaps/Abstractions/MpBeatmapLevel.hpp"
#include "Beatmaps/Packets/MpBeatmapPacket.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CUSTOM_DLL(MultiplayerCore::Beatmaps, NetworkBeatmapLevel, Abstractions::MpBeatmapLevel, "MultiplayerCore.Beatmaps",
//___DECLARE_TYPE_WRAPPER_INHERITANCE(MultiplayerCore::Beatmaps, NetworkBeatmapLevel, Il2CppTypeEnum::IL2CPP_TYPE_CLASS,
//	Il2CppObject, "MultiplayerCore.Beatmaps",
//	{ classof(MpBeatmapPacket) }, 0, nullptr,
	DECLARE_CTOR(New, Packets::MpBeatmapPacket* packet);

	//// Packet
	DECLARE_INSTANCE_FIELD(Packets::MpBeatmapPacket*, _packet);

	DECLARE_INSTANCE_FIELD(System::Threading::Tasks::Task_1<UnityEngine::Sprite*>*, coverImageTask);

	// Getter overrides
	DECLARE_OVERRIDE_METHOD(StringW, get_levelHash, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());

	DECLARE_OVERRIDE_METHOD(StringW, get_levelID, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelID>::get());

	//DECLARE_OVERRIDE_METHOD(StringW, get_levelID, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songSubName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_levelAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(float, get_beatsPerMinute, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(float, get_songDuration, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());

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