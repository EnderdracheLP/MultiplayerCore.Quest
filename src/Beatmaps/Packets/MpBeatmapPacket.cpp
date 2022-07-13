#include "main.hpp"
#include "custom-types/shared/register.hpp"
#include "Beatmaps/Packets/MpBeatmapPacket.hpp"
#include "GlobalNamespace/VarIntExtensions.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
using namespace GlobalNamespace;

#include "Utilities.hpp"
using namespace MultiplayerCore;

DEFINE_TYPE(MultiplayerCore::Beatmaps::Packets, MpBeatmapPacket);

namespace MultiplayerCore::Beatmaps::Packets {

	void MpBeatmapPacket::New() {}

	MpBeatmapPacket* MpBeatmapPacket::CS_Ctor(GlobalNamespace::PreviewDifficultyBeatmap* beatmap) {
		MpBeatmapPacket* packet = MpBeatmapPacket::New_ctor();
		packet->levelHash = Utilities::HashForLevelID(beatmap->get_beatmapLevel()->get_levelID());
		packet->songName = beatmap->get_beatmapLevel()->get_songName();
		packet->songSubName = beatmap->get_beatmapLevel()->get_songSubName();
		packet->songAuthorName = beatmap->get_beatmapLevel()->get_songAuthorName();
		packet->levelAuthorName = beatmap->get_beatmapLevel()->get_levelAuthorName();
		packet->beatsPerMinute = beatmap->get_beatmapLevel()->get_beatsPerMinute();
		packet->songDuration = beatmap->get_beatmapLevel()->get_songDuration();

		packet->characteristic = beatmap->get_beatmapCharacteristic()->get_serializedName();
		packet->difficulty = beatmap->get_beatmapDifficulty();
		return packet;
	}

	void MpBeatmapPacket::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
		getLogger().debug("MpBeatmapPacket::Serialize");

		writer->Put(levelHash);
		writer->Put(songName);
		writer->Put(songSubName);
		writer->Put(songAuthorName);
		writer->Put(levelAuthorName);
		writer->Put(beatsPerMinute);
		writer->Put(songDuration);

		writer->Put(characteristic);
		// writer->Put((uint)difficulty);
		GlobalNamespace::VarIntExtensions::PutVarUInt(writer, (uint)difficulty);

		// TODO: Properly add data, for now we just send empty data
		writer->Put((uint8_t)0); // requirements Count
		// for loop here
		writer->Put((uint8_t)0); // contributors Length

		writer->Put((uint8_t)0); // mapColors Count
	}

	void MpBeatmapPacket::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
		getLogger().debug("MpBeatmapPacket::Deserialize");

		levelHash = reader->GetString();
		songName = reader->GetString();
		songSubName = reader->GetString();
		songAuthorName = reader->GetString();
		levelAuthorName = reader->GetString();
		beatsPerMinute = reader->GetFloat();
		songDuration = reader->GetFloat();

		characteristic = reader->GetString();
		difficulty = (BeatmapDifficulty)GlobalNamespace::VarIntExtensions::GetVarUInt(reader);
		getLogger().debug("Deserialize MpBeatmapPacket done");
	}

}
