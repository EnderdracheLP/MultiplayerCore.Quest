#include "Beatmaps/NetworkBeatmapLevel.hpp"
#include "custom-types/shared/register.hpp"
#include "GlobalNamespace/VarIntExtensions.hpp"

DEFINE_TYPE(MultiplayerCore::Beatmaps, NetworkBeatmapLevel);

namespace MultiplayerCore::Beatmaps {
	void NetworkBeatmapLevel::New(Packets::MpBeatmapPacket* packet) {
		//levelHash = packet->levelHash;

		_packet = packet;

		//songName = packet->songName;
		//songSubName = packet->songSubName;
		//songAuthorName = packet->songAuthorName;
		//levelAuthorName = packet->levelAuthorName;
		//beatsPerMinute = packet->beatsPerMinute;
		//songDuration = packet->songDuration;
	}

#pragma region All the pain in form of getter functions

	StringW NetworkBeatmapLevel::get_levelHash() {
		return _packet->levelHash;
	}

	StringW NetworkBeatmapLevel::get_songName() {
		return _packet->songName;
	}

	StringW NetworkBeatmapLevel::get_songSubName() {
		return _packet->songSubName;
	}

	StringW NetworkBeatmapLevel::get_songAuthorName() {
		return _packet->songAuthorName;
	}

	StringW NetworkBeatmapLevel::get_levelAuthorName() {
		return _packet->levelAuthorName;
	}

	float NetworkBeatmapLevel::get_beatsPerMinute() {
		return _packet->beatsPerMinute;
	}

	float NetworkBeatmapLevel::get_songDuration() {
		return _packet->songDuration;
	}
#pragma endregion
}
