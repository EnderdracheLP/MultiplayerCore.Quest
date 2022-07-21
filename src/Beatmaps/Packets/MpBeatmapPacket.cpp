#include "main.hpp"
#include "custom-types/shared/register.hpp"
#include "Beatmaps/Packets/MpBeatmapPacket.hpp"
#include "GlobalNamespace/VarIntExtensions.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
using namespace GlobalNamespace;

#include "Utilities.hpp"
#include "CodegenExtensions/EnumUtils.hpp"
using namespace MultiplayerCore;

DEFINE_TYPE(MultiplayerCore::Beatmaps::Packets, MpBeatmapPacket);

namespace MultiplayerCore::Beatmaps::Packets {

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
		writer->Put((uint)difficulty);
		// GlobalNamespace::VarIntExtensions::PutVarUInt(writer, (uint)difficulty);

		// TODO: Properly add data, for now we just send empty data
		writer->Put((uint8_t)0); // requirements Count
		// for loop here
		writer->Put((uint8_t)0); // contributors Length

		writer->Put((uint8_t)0); // mapColors Count
	}

	void MpBeatmapPacket::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
		getLogger().debug("MpBeatmapPacket::Deserialize: Start");

		levelHash = reader->GetString();
		songName = reader->GetString();
		songSubName = reader->GetString();
		songAuthorName = reader->GetString();
		levelAuthorName = reader->GetString();
		beatsPerMinute = reader->GetFloat();
		songDuration = reader->GetFloat();

		characteristic = reader->GetString();
		difficulty = (BeatmapDifficulty)reader->GetUInt();

		// New data to deserialize, we try catch it here so failing to deserialize doesn't invalidate the packet
		try {
			getLogger().debug("MpBeatmapPacket::Deserialize: New data to deserialize, reading difficulty requirements");
			uint8_t difficultyCount = reader->GetByte();
			getLogger().debug("MpBeatmapPacket::Deserialize: Difficulty requirements count: %d", difficultyCount);
			for (uint8_t i = 0; i < difficultyCount; i++) {
				getLogger().debug("MpBeatmapPacket::Deserialize: Reading difficulty requirement %d", i);
				uint8_t difficulty = reader->GetByte(); // This can be cast to type BeatmapDifficulty
				getLogger().debug("MpBeatmapPacket::Deserialize: Difficulty requirement %s", EnumUtils::GetEnumName((BeatmapDifficulty)difficulty).c_str());
				uint8_t requirementCount = reader->GetByte();
				getLogger().debug("MpBeatmapPacket::Deserialize: Difficulty requirements count: %d", requirementCount);
				std::vector<StringW> reqsForDifficulty;
				for (uint8_t j = 0; j < requirementCount; j++) {
					getLogger().debug("MpBeatmapPacket::Deserialize: Reading difficulty requirement %d", j);
					reqsForDifficulty.push_back(reader->GetString());
					getLogger().debug("MpBeatmapPacket::Deserialize: Difficulty: %s, Requirement: %s", EnumUtils::GetEnumName((BeatmapDifficulty)difficulty).c_str(), static_cast<std::string>(reqsForDifficulty[j]).c_str());
					// getLogger().debug("MpBeatmapPacket::Deserialize: Difficulty: %s, Requirement: %s", EnumUtils::GetEnumName((BeatmapDifficulty)difficulty).c_str(), reqsForDifficulty[j].c_str());
				}
				getLogger().debug("MpBeatmapPacket::Deserialize: Assigning reqsForDifficulty");
				requirements.insert_or_assign(difficulty, reqsForDifficulty);
				getLogger().debug("MpBeatmapPacket::Deserialize: Assigned reqsForDifficulty");
			}
		}
		catch(::std::exception const& exc) {
			getLogger().error("MpBeatmapPacket::Deserialize: Error reading data, is the players MpCore version up to date?");
			getLogger().error("std::exception REPORT TO ENDER: %s", exc.what());
		}


		getLogger().debug("MpBeatmapPacket::Deserialize: done");
	}

}
