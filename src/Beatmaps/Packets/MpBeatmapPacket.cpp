#include "Beatmaps/Packets/MpBeatmapPacket.hpp"
#include "Beatmaps/Abstractions/MpBeatmapLevel.hpp"
#include "Utilities.hpp"

#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"

DEFINE_TYPE(MultiplayerCore::Beatmaps::Packets, MpBeatmapPacket);

namespace MultiplayerCore::Beatmaps::Packets {
    void MpBeatmapPacket::New() {
        INVOKE_CTOR();
        INVOKE_BASE_CTOR(classof(MultiplayerCore::Networking::Abstractions::MpPacket*));
    }

    MpBeatmapPacket* MpBeatmapPacket::New_1(GlobalNamespace::PreviewDifficultyBeatmap* beatmap) {
        auto packet = MpBeatmapPacket::New_ctor();

        auto beatmapLevel = beatmap->get_beatmapLevel();
        packet->levelHash = Utilities::HashForLevelId(beatmapLevel->get_levelID());
        packet->songName = beatmapLevel->get_songName();
        packet->songSubName = beatmapLevel->get_songSubName();
        packet->songAuthorName = beatmapLevel->get_songAuthorName();
        packet->levelAuthorName = beatmapLevel->get_levelAuthorName();
        packet->beatsPerMinute = beatmapLevel->get_beatsPerMinute();
        packet->songDuration = beatmapLevel->get_songDuration();

        auto ch = beatmap->get_beatmapCharacteristic();
        packet->characteristic = ch->get_serializedName();
        packet->difficulty = beatmap->get_beatmapDifficulty();

        auto mpBeatmapLevel = il2cpp_utils::try_cast<Abstractions::MpBeatmapLevel>(beatmapLevel).value_or(nullptr);
        if (mpBeatmapLevel) {
            auto req = mpBeatmapLevel->requirements.find(ch->get_name());
            if (req != mpBeatmapLevel->requirements.end()) packet->requirements = req->second;
            req = mpBeatmapLevel->requirements.find(ch->get_serializedName());
            if (req != mpBeatmapLevel->requirements.end()) packet->requirements = req->second;
            packet->contributors.clear();
            packet->contributors.reserve(mpBeatmapLevel->contributors.size());
            for (const auto& c : mpBeatmapLevel->contributors) packet->contributors.emplace_back(c);
        }

        return packet;
    }

    void MpBeatmapPacket::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
        writer->Put(levelHash);
        writer->Put(songName);
        writer->Put(songSubName);
        writer->Put(songAuthorName);
        writer->Put(levelAuthorName);
        writer->Put(beatsPerMinute);
        writer->Put(songDuration);

        writer->Put(characteristic);
        writer->Put((uint32_t)difficulty.value__);

        writer->Put((uint8_t)requirements.size());
        for (const auto& [key, value] : requirements) {
            writer->Put(uint8_t(key));
            writer->Put(uint8_t(value.size()));
            for (const auto& req : value)
                writer->Put(StringW(req));
        }

        // on quest we can just read contributors size as it can't be null
        writer->Put(uint8_t(contributors.size()));
        for (const auto& [role, name, iconPath] : contributors) {
            writer->Put(StringW(role));
            writer->Put(StringW(name));
            writer->Put(StringW(iconPath));
        }

        writer->Put(uint8_t(mapColors.size()));
        for (const auto& [key, value] : mapColors) {
            writer->Put(uint8_t(key));
            value.Serialize(writer);
        }
    }

    void MpBeatmapPacket::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
        levelHash = reader->GetString();
        songName = reader->GetString();
        songSubName = reader->GetString();
        songAuthorName = reader->GetString();
        levelAuthorName = reader->GetString();
        beatsPerMinute = reader->GetFloat();
        songDuration = reader->GetFloat();

        characteristic = reader->GetString();
        difficulty = reader->GetUInt();

        auto difficultyCount = reader->GetByte();
        for (std::size_t i = 0; i < difficultyCount; i++) {
            auto difficulty = reader->GetByte();
            auto reqCount = reader->GetByte();
            auto& reqsForDifficulty = requirements[difficulty];
            for (std::size_t j = 0; j < reqCount; j++)
                reqsForDifficulty.emplace_back(reader->GetString());
        }

        auto contributorCount = reader->GetByte();
        for (std::size_t i = 0; i < contributorCount; i++)
            contributors.emplace_back(
                reader->GetString(),
                reader->GetString(),
                reader->GetString()
            );

        auto colorCount = reader->GetByte();
        for (std::size_t i = 0; i < colorCount; i++) {
            auto difficulty = reader->GetByte();
            mapColors[difficulty].Deserialize(reader);
        }
    }
}
