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

	//DECLARE_INSTANCE_FIELD(StringW, songName);
	//DECLARE_INSTANCE_FIELD(StringW, songSubName);
	//DECLARE_INSTANCE_FIELD(StringW, songAuthorName);
	//DECLARE_INSTANCE_FIELD(StringW, levelAuthorName);
	//DECLARE_INSTANCE_FIELD(float, beatsPerMinute);
	//DECLARE_INSTANCE_FIELD(float, songDuration);

	//// Packet
	DECLARE_INSTANCE_FIELD(Packets::MpBeatmapPacket*, _packet);

	// Getter overrides
	DECLARE_OVERRIDE_METHOD(StringW, get_levelHash, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());

	//DECLARE_OVERRIDE_METHOD(StringW, get_levelID, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songSubName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_levelAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(float, get_beatsPerMinute, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(float, get_songDuration, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());


	public:
		// Conversion operators to BaseType MpBeatmapLevel
		operator Abstractions::MpBeatmapLevel* () noexcept {
			return reinterpret_cast<Abstractions::MpBeatmapLevel*>(this);
		}

)