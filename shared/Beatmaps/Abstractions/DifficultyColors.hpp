#pragma once
#include "custom-types/shared/macros.hpp"
#include "Networking/Abstractions/MpPacket.hpp"
#include "Utils/ExtraSongData.hpp"

// DECLARE_CLASS_CUSTOM_DLL(MultiplayerCore::Beatmaps::Abstractions, DifficultyColors, MultiplayerCore::Networking::Abstractions::MpPacket, "MultiplayerCore.Beatmaps.Abstractions",
//     DECLARE_DEFAULT_CTOR();

//     DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&LiteNetLib::Utils::INetSerializable::Serialize>::get(), LiteNetLib::Utils::NetDataWriter* writer);
//     DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&LiteNetLib::Utils::INetSerializable::Deserialize>::get(), LiteNetLib::Utils::NetDataReader* reader);

//     public:
//         UnityEngine::Color _colorLeft;
//         UnityEngine::Color _colorRight;
//         UnityEngine::Color _envColorLeft;
//         UnityEngine::Color _envColorRight;
//         UnityEngine::Color _envColorLeftBoost;
//         UnityEngine::Color _envColorRightBoost;
//         UnityEngine::Color _obstacleColor;

//         template<::il2cpp_utils::CreationType creationType = ::il2cpp_utils::CreationType::Temporary>
//         static inline DifficultyColors* New_ctor(UnityEngine::Color colorLeft, UnityEngine::Color colorRight, 
//             UnityEngine::Color envColorLeft, UnityEngine::Color envColorRight, 
//             UnityEngine::Color envColorLeftBoost, UnityEngine::Color envColorRightBoost,
//             UnityEngine::Color obstacleColor)
//         {
//             DifficultyColors* instance = MultiplayerCore::Beatmaps::Abstractions::DifficultyColors::New_ctor<creationType>();
//             instance->colorLeft = colorLeft;
//             instance->colorRight = colorRight;
//             instance->envColorLeft = envColorLeft;
//             instance->envColorRight = envColorRight;
//             instance->envColorLeftBoost = envColorLeftBoost;
//             instance->envColorRightBoost = envColorRightBoost;
//             instance->obstacleColor = obstacleColor;
//             return instance;
//         }

//         static inline void SerializeMapColor(LiteNetLib::Utils::NetDataWriter* writer, UnityEngine::Color color)
//         {
//             writer->Put(color.r);
//             writer->Put(color.g);
//             writer->Put(color.b);
//         }

//         static inline UnityEngine::Color DeserializeMapColor(LiteNetLib::Utils::NetDataReader* reader)
//         {
//             UnityEngine::Color color;
//             color.r = reader->GetFloat();
//             color.g = reader->GetFloat();
//             color.b = reader->GetFloat();
//         }
// )

namespace MultiplayerCore::Beatmaps::Abstractions {
    struct DifficultyColors {
        
        std::optional<MultiplayerCore::Utils::ExtraSongData::MapColor> _colorLeft;
        std::optional<MultiplayerCore::Utils::ExtraSongData::MapColor> _colorRight;
        std::optional<MultiplayerCore::Utils::ExtraSongData::MapColor> _envColorLeft;
        std::optional<MultiplayerCore::Utils::ExtraSongData::MapColor> _envColorRight;
        std::optional<MultiplayerCore::Utils::ExtraSongData::MapColor> _envColorLeftBoost;
        std::optional<MultiplayerCore::Utils::ExtraSongData::MapColor> _envColorRightBoost;
        std::optional<MultiplayerCore::Utils::ExtraSongData::MapColor> _obstacleColor;

        void Serialize(LiteNetLib::Utils::NetDataWriter* writer);
        void Deserialize(LiteNetLib::Utils::NetDataReader* reader);

        constexpr DifficultyColors() {}
        inline DifficultyColors(MultiplayerCore::Utils::ExtraSongData::DifficultyData data) {
            _colorLeft = data._colorLeft;
            _colorRight = data._colorRight;
            _envColorLeft = data._envColorLeft;
            _envColorRight = data._envColorRight;
            _envColorLeftBoost = data._envColorLeftBoost;
            _envColorRightBoost = data._envColorRightBoost;
            _obstacleColor = data._obstacleColor;
        }
        constexpr DifficultyColors(MultiplayerCore::Utils::ExtraSongData::MapColor colorLeft, MultiplayerCore::Utils::ExtraSongData::MapColor colorRight, 
            MultiplayerCore::Utils::ExtraSongData::MapColor envColorLeft, MultiplayerCore::Utils::ExtraSongData::MapColor envColorRight, 
            MultiplayerCore::Utils::ExtraSongData::MapColor envColorLeftBoost, MultiplayerCore::Utils::ExtraSongData::MapColor envColorRightBoost,
            MultiplayerCore::Utils::ExtraSongData::MapColor obstacleColor)
        {
            this->_colorLeft = colorLeft;
            this->_colorRight = colorRight;
            this->_envColorLeft = envColorLeft;
            this->_envColorRight = envColorRight;
            this->_envColorLeftBoost = envColorLeftBoost;
            this->_envColorRightBoost = envColorRightBoost;
            this->_obstacleColor = obstacleColor;
        }

        static inline void SerializeMapColor(LiteNetLib::Utils::NetDataWriter* writer, UnityEngine::Color color)
        {
            writer->Put(color.r);
            writer->Put(color.g);
            writer->Put(color.b);
        }

        static inline MultiplayerCore::Utils::ExtraSongData::MapColor DeserializeMapColor(LiteNetLib::Utils::NetDataReader* reader)
        {
            return MultiplayerCore::Utils::ExtraSongData::MapColor(reader);
        }
    };
}