#pragma once
#include "main.hpp"
#include "Beatmaps/Packets/MpBeatmapPacket.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

___DECLARE_TYPE_WRAPPER_INHERITANCE(MultiplayerCore::Beatmaps, NetworkBeatmapLevel, Il2CppTypeEnum::IL2CPP_TYPE_CLASS,
	Il2CppObject, "MultiplayerCore.Beatmaps",
	{ classof(MpBeatmapPacket) }, 0, nullptr,
	DECLARE_CTOR(New, MultiplayerCore::Packets::MpBeatmapPacket packet);

	// Packet
	DECLARE_INSTANCE_FIELD(MultiplayerCore::Packets::MpBeatmapPacket, _packet);

	// Getter overrides
	DECLARE_INSTANCE_METHOD(StringW, get_levelHash, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());

	DECLARE_OVERRIDE_METHOD(StringW, get_levelID, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songSubName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_levelAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(float, get_beatsPerMinute, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(float, get_songDuration, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
)
