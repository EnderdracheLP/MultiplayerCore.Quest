#include "main.hpp"
#include "custom-types/shared/register.hpp"
#include "Beatmaps/Packets/MpBeatmapPacket.hpp"
#include "GlobalNamespace/VarIntExtensions.hpp"
using namespace GlobalNamespace;

#include "Utilities.hpp"
using namespace MultiplayerCore;

DEFINE_TYPE(MultiplayerCore::Beatmaps::Packets, MpBeatmapPacket);

namespace MultiplayerCore::Beatmaps::Packets {

	//void PreviewBeatmapPacket::New_ctor() {}
	//PreviewBeatmapPacket::PreviewBeatmapPacket() {}
	void MpBeatmapPacket::New(GlobalNamespace::IPreviewBeatmapLevel* preview, StringW characteristic, GlobalNamespace::BeatmapDifficulty difficulty) {
		levelHash = Utilities::HashForLevelID(preview->get_levelID());
		songName = preview->get_songName();
		songSubName = preview->get_songSubName();
		songAuthorName = preview->get_songAuthorName();
		levelAuthorName = preview->get_levelAuthorName();
		beatsPerMinute = preview->get_beatsPerMinute();
		songDuration = preview->get_songDuration();

		this->characteristic = characteristic;
		this->difficulty = difficulty;
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
		GlobalNamespace::VarIntExtensions::PutVarUInt(writer, (uint)difficulty);
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
