#pragma once

#include "Abstractions/MpBeatmapLevel.hpp"
#include "Packets/MpBeatmapPacket.hpp"

DECLARE_CLASS_CUSTOM(MultiplayerCore::Beatmaps, NetworkBeatmapLevel, Abstractions::MpBeatmapLevel,
    DECLARE_INSTANCE_FIELD_PRIVATE(Packets::MpBeatmapPacket*, _packet);

    DECLARE_OVERRIDE_METHOD(StringW, get_songName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songName>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songSubName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songSubName>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songAuthorName>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_levelAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_levelAuthorName>::get());

	DECLARE_OVERRIDE_METHOD(float, get_beatsPerMinute, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_beatsPerMinute>::get());
	DECLARE_OVERRIDE_METHOD(float, get_songDuration, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::get_songDuration>::get());

	DECLARE_INSTANCE_FIELD_PRIVATE(System::Threading::Tasks::Task_1<UnityEngine::Sprite*>*, coverImageTask);
	DECLARE_OVERRIDE_METHOD(System::Threading::Tasks::Task_1<UnityEngine::Sprite*>*, GetCoverImageAsync, il2cpp_utils::il2cpp_type_check::MetadataGetter<&GlobalNamespace::IPreviewBeatmapLevel::GetCoverImageAsync>::get(), System::Threading::CancellationToken cancellationToken);

    DECLARE_CTOR(ctor_1, Packets::MpBeatmapPacket*);

    public:
        const std::unordered_map<uint8_t, std::list<std::string>>& get_requirements() { return _packet->requirements; };
        const std::unordered_map<uint8_t, MultiplayerCore::Beatmaps::Abstractions::DifficultyColors>& get_mapColors() { return _packet->mapColors; };
        const std::vector<const MultiplayerCore::Utils::ExtraSongData::Contributor>& get_contributors() { return _packet->contributors; };
)
