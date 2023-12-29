#pragma once

#include "Abstractions/MpBeatmapLevel.hpp"
#include "Packets/MpBeatmapPacket.hpp"
#include "songdownloader/shared/Types/BeatSaver/Beatmap.hpp"

DECLARE_CLASS_CUSTOM(MultiplayerCore::Beatmaps, NetworkBeatmapLevel, Abstractions::MpBeatmapLevel,
    DECLARE_INSTANCE_FIELD_PRIVATE(Packets::MpBeatmapPacket*, _packet);

    DECLARE_OVERRIDE_METHOD_MATCH(StringW, get_songName, &GlobalNamespace::IPreviewBeatmapLevel::get_songName);
	DECLARE_OVERRIDE_METHOD_MATCH(StringW, get_songSubName, &GlobalNamespace::IPreviewBeatmapLevel::get_songSubName);
	DECLARE_OVERRIDE_METHOD_MATCH(StringW, get_songAuthorName, &GlobalNamespace::IPreviewBeatmapLevel::get_songAuthorName);
	DECLARE_OVERRIDE_METHOD_MATCH(StringW, get_levelAuthorName, &GlobalNamespace::IPreviewBeatmapLevel::get_levelAuthorName);

	DECLARE_OVERRIDE_METHOD_MATCH(float, get_beatsPerMinute, &GlobalNamespace::IPreviewBeatmapLevel::get_beatsPerMinute);
	DECLARE_OVERRIDE_METHOD_MATCH(float, get_songDuration, &GlobalNamespace::IPreviewBeatmapLevel::get_songDuration);

	DECLARE_INSTANCE_FIELD_PRIVATE(System::Threading::Tasks::Task_1<UnityEngine::Sprite*>*, coverImageTask);
	DECLARE_OVERRIDE_METHOD_MATCH(System::Threading::Tasks::Task_1<UnityEngine::Sprite*>*, GetCoverImageAsync, &GlobalNamespace::IPreviewBeatmapLevel::GetCoverImageAsync, System::Threading::CancellationToken cancellationToken);

    DECLARE_CTOR(ctor_1, Packets::MpBeatmapPacket*);

    public:
        const std::unordered_map<uint8_t, std::list<std::string>>& get_requirements() { return _packet->requirements; };
        const std::unordered_map<uint8_t, MultiplayerCore::Beatmaps::Abstractions::DifficultyColors>& get_mapColors() { return _packet->mapColors; };
        const std::vector<const MultiplayerCore::Utils::ExtraSongData::Contributor>& get_contributors() { return _packet->contributors; };
    private:
        void OnGetBeatmapComplete(std::__ndk1::optional<BeatSaver::Beatmap> beatmapOpt);
        void OnGetCoverImageComplete(std::vector<uint8_t> bytes);
)

#undef SONGDOWNLOADER_INCLUDED
