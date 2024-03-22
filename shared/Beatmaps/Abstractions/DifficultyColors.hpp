#pragma once

#include "../../_config.h"
#include "../../Utils/ExtraSongData.hpp"

namespace MultiplayerCore::Beatmaps::Abstractions {
    using ColorOpt = std::optional<MultiplayerCore::Utils::ExtraSongData::MapColor>;
    struct MPCORE_EXPORT DifficultyColors {
        ColorOpt colorLeft;
        ColorOpt colorRight;
        ColorOpt envColorLeft;
        ColorOpt envColorRight;
        ColorOpt envColorLeftBoost;
        ColorOpt envColorRightBoost;
        ColorOpt obstacleColor;

        void Serialize(LiteNetLib::Utils::NetDataWriter* writer) const;
        void Deserialize(LiteNetLib::Utils::NetDataReader* reader);

        constexpr DifficultyColors() {}
        inline DifficultyColors(const Utils::ExtraSongData::DifficultyData& data) {
            colorLeft = data.colorLeft;
            colorRight = data.colorRight;
            envColorLeft = data.envColorLeft;
            envColorRight = data.envColorRight;
            envColorLeftBoost = data.envColorLeftBoost;
            envColorRightBoost = data.envColorRightBoost;
            obstacleColor = data.obstacleColor;
        }
        constexpr DifficultyColors(
            MultiplayerCore::Utils::ExtraSongData::MapColor colorLeft,
            MultiplayerCore::Utils::ExtraSongData::MapColor colorRight,
            MultiplayerCore::Utils::ExtraSongData::MapColor envColorLeft,
            MultiplayerCore::Utils::ExtraSongData::MapColor envColorRight,
            MultiplayerCore::Utils::ExtraSongData::MapColor envColorLeftBoost,
            MultiplayerCore::Utils::ExtraSongData::MapColor envColorRightBoost,
            MultiplayerCore::Utils::ExtraSongData::MapColor obstacleColor
        ) :
            colorLeft(colorLeft),
            colorRight(colorRight),
            envColorLeft(envColorLeft),
            envColorRight(envColorRight),
            envColorLeftBoost(envColorLeftBoost),
            envColorRightBoost(envColorRightBoost),
            obstacleColor(obstacleColor)
        {}

        static inline void SerializeMapColor(LiteNetLib::Utils::NetDataWriter* writer, const MultiplayerCore::Utils::ExtraSongData::MapColor color) {
            color.Serialize(writer);
        }

        static inline void SerializeMapColor(LiteNetLib::Utils::NetDataWriter* writer, const UnityEngine::Color color) {
            writer->Put(color.r);
            writer->Put(color.g);
            writer->Put(color.b);
        }

        static inline MultiplayerCore::Utils::ExtraSongData::MapColor DeserializeMapColor(LiteNetLib::Utils::NetDataReader* reader) {
            return MultiplayerCore::Utils::ExtraSongData::MapColor(reader);
        }

        constexpr bool AnyAreNotNull() const {
            return
                colorLeft.has_value() ||
                colorRight.has_value() ||
                envColorLeft.has_value() ||
                envColorRight.has_value() ||
                envColorLeftBoost.has_value() ||
                envColorRightBoost.has_value() ||
                obstacleColor.has_value();
        }
    };
}
