#include "Beatmaps/Abstractions/DifficultyColors.hpp"

namespace MultiplayerCore::Beatmaps::Abstractions {
    void DifficultyColors::Serialize(LiteNetLib::Utils::NetDataWriter* writer) const {
        uint8_t colors = 0;
        colors |= colorLeft.has_value() << 0;
        colors |= colorRight.has_value() << 1;
        colors |= envColorLeft.has_value() << 2;
        colors |= envColorRight.has_value() << 3;
        colors |= envColorLeftBoost.has_value() << 4;
        colors |= envColorRightBoost.has_value() << 5;
        colors |= obstacleColor.has_value() << 6;
        writer->Put(colors);

        if (colorLeft.has_value()) colorLeft->Serialize(writer);
        if (colorRight.has_value()) colorRight->Serialize(writer);
        if (envColorLeft.has_value()) envColorLeft->Serialize(writer);
        if (envColorRight.has_value()) envColorRight->Serialize(writer);
        if (envColorLeftBoost.has_value()) envColorLeftBoost->Serialize(writer);
        if (envColorRightBoost.has_value()) envColorRightBoost->Serialize(writer);
        if (obstacleColor.has_value()) obstacleColor->Serialize(writer);
    }

    void DifficultyColors::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
        uint8_t colors = reader->GetByte();
        if (((colors << 0) & 0x1) != 0) colorLeft = MultiplayerCore::Utils::ExtraSongData::MapColor(reader);
        if (((colors << 1) & 0x1) != 0) colorRight = MultiplayerCore::Utils::ExtraSongData::MapColor(reader);
        if (((colors << 2) & 0x1) != 0) envColorLeft = MultiplayerCore::Utils::ExtraSongData::MapColor(reader);
        if (((colors << 3) & 0x1) != 0) envColorRight = MultiplayerCore::Utils::ExtraSongData::MapColor(reader);
        if (((colors << 4) & 0x1) != 0) envColorLeftBoost = MultiplayerCore::Utils::ExtraSongData::MapColor(reader);
        if (((colors << 5) & 0x1) != 0) envColorRightBoost = MultiplayerCore::Utils::ExtraSongData::MapColor(reader);
        if (((colors << 6) & 0x1) != 0) obstacleColor = MultiplayerCore::Utils::ExtraSongData::MapColor(reader);
    }
}
