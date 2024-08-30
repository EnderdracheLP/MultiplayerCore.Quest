#include "Beatmaps/Packets/MpBeatmapPacket.hpp"
#include "Beatmaps/Abstractions/MpBeatmapLevel.hpp"

#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "songcore/shared/SongLoader/RuntimeSongLoader.hpp"
#include "Utilities.hpp"
#include "logging.hpp"

DEFINE_TYPE(MultiplayerCore::Beatmaps::Packets, MpBeatmapPacket);

namespace MultiplayerCore::Beatmaps::Packets {
    void MpBeatmapPacket::New() {
        INVOKE_CTOR();
        INVOKE_BASE_CTOR(classof(MultiplayerCore::Networking::Abstractions::MpPacket*));
    }

    MpBeatmapPacket* MpBeatmapPacket::New_1(GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::BeatmapKey beatmapKey) {
        auto packet = MpBeatmapPacket::New_ctor();

        packet->levelHash = HashFromLevelID(beatmapLevel->levelID);
        packet->songName = beatmapLevel->songName;
        packet->songSubName = beatmapLevel->songSubName;
        packet->songAuthorName = beatmapLevel->songAuthorName;
        packet->levelAuthorName = beatmapLevel->allMappers ? beatmapLevel->allMappers.front().value_or("") : "";
        packet->beatsPerMinute = beatmapLevel->beatsPerMinute;
        packet->songDuration = beatmapLevel->songDuration;

        packet->characteristic = beatmapKey.beatmapCharacteristic->serializedName;
        packet->difficulty = beatmapKey.difficulty;

        auto mpBeatmapLevel = il2cpp_utils::try_cast<Abstractions::MpBeatmapLevel>(beatmapLevel).value_or(nullptr);
        if (mpBeatmapLevel) {
            auto req = mpBeatmapLevel->requirements.find(packet->characteristic);
            if (req != mpBeatmapLevel->requirements.end()) packet->requirements = req->second;
            else {
                DEBUG("Found no requirements for level {}", packet->levelHash);
                packet->requirements.emplace(beatmapKey.difficulty.value__, std::list<std::string>());
            }
            packet->contributors.clear();
            packet->contributors.reserve(mpBeatmapLevel->contributors.size());
            for (const auto& c : mpBeatmapLevel->contributors) packet->contributors.emplace_back(c);
        } else {
            WARNING("BeatmapLevel not MpCore type: {}", packet->levelHash);
            packet->requirements.emplace(beatmapKey.difficulty.value__, std::list<std::string>());
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
