#pragma once
#include <string>
#include <string_view>
#include <optional>
#include <vector>

#include "EnumUtils.hpp"

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "songloader/shared/API.hpp"

#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/Color.hpp"

#include "LiteNetLib/Utils/NetDataReader.hpp"
#include "LiteNetLib/Utils/NetDataWriter.hpp"

#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"

//Temporary include
#include "main.hpp"

namespace MultiplayerCore::Utils {
    struct ExtraSongData {
        struct Contributor {
            Contributor(std::string name, std::string role, std::string iconPath) :
                name(name), role(role), iconPath(iconPath) {}
            const std::string role;
            const std::string name;
            const std::string iconPath;

            // UnityEngine::Sprite* icon;
        };

        struct RequirementData
		{
            RequirementData(std::vector<std::string> requirements, std::vector<std::string> suggestions, std::vector<std::string> warnings, std::vector<std::string> information) :
                _requirements(std::move(requirements)), _suggestions(std::move(suggestions)), _warnings(std::move(warnings)), _information(std::move(information)) {}
			std::vector<std::string> _requirements;

			std::vector<std::string> _suggestions;

			std::vector<std::string> _warnings;

			std::vector<std::string> _information;
		};

		struct MapColor : public UnityEngine::Color {
            constexpr MapColor() : UnityEngine::Color() {}
            MapColor(float r, float g, float b) : UnityEngine::Color(r, g, b, 1.0f) {}
            inline MapColor(rapidjson::Value::Object json) {
                auto rItr = json.FindMember("r");
                r = rItr != json.MemberEnd() ? rItr->value.GetFloat() : 1.0f;
                auto gItr = json.FindMember("g");
                g = gItr != json.MemberEnd() ? gItr->value.GetFloat() : 1.0f;
                auto bItr = json.FindMember("b");
                b = bItr != json.MemberEnd() ? bItr->value.GetFloat() : 1.0f;
            }
            MapColor(LiteNetLib::Utils::NetDataReader* reader) : UnityEngine::Color(reader->GetFloat(), reader->GetFloat(), reader->GetFloat(), 1.0f) {}
            inline void Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
                writer->Put(r);
                writer->Put(g);
                writer->Put(b);
            }
            inline void Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
                r = reader->GetFloat();
                g = reader->GetFloat();
                b = reader->GetFloat();
            }
        };

        struct DifficultyData {
            DifficultyData() {}
            DifficultyData(std::string_view beatmapCharacteristicName, 
            GlobalNamespace::BeatmapDifficulty difficulty, std::optional<std::string_view> difficultyLabel, 
            std::optional<RequirementData> additionalDifficultyData,
            MapColor colorLeft, MapColor colorRight, 
            MapColor envColorLeft, MapColor envColorRight,
            MapColor envColorLeftBoost, MapColor envColorRightBoost,
            MapColor obstacleColor) :
                _beatmapCharacteristicName(beatmapCharacteristicName), 
                _difficultyLabel(difficultyLabel), _difficulty(difficulty), 
                additionalDifficultyData(additionalDifficultyData),
                _colorLeft(colorLeft), _colorRight(colorRight),
                _envColorLeft(envColorLeft), _envColorRight(envColorRight),
                _envColorLeftBoost(envColorLeftBoost), _envColorRightBoost(envColorRightBoost),
                _obstacleColor(obstacleColor) {}

            std::string _beatmapCharacteristicName;

			GlobalNamespace::BeatmapDifficulty _difficulty;

			std::optional<std::string_view> _difficultyLabel;

			std::optional<RequirementData> additionalDifficultyData;

			MapColor _colorLeft;

			MapColor _colorRight;

			MapColor _envColorLeft;

			MapColor _envColorRight;

			MapColor _envColorLeftBoost;

			MapColor _envColorRightBoost;

			MapColor _obstacleColor;
        };

        std::vector<const Contributor> contributors;

        std::string _customEnvironmentName;

        std::string _customEnvironmentHash;

        std::vector<const DifficultyData> _difficulties;

        std::string _defaultCharacteristicName;

        static inline std::optional<ExtraSongData> FromLevelId(std::string levelId)
        {
            auto customPreview = RuntimeSongLoader::API::GetLevelById(levelId);
            if (customPreview)
            {
                return FromMapPath((*customPreview)->customLevelPath);
            }
            else
            {
                return std::nullopt;
            }
        }

        static inline std::optional<ExtraSongData> FromPreviewBeatmapLevel(GlobalNamespace::IPreviewBeatmapLevel* preview)
        {
            std::optional<GlobalNamespace::CustomPreviewBeatmapLevel*> customPreview = il2cpp_utils::try_cast<GlobalNamespace::CustomPreviewBeatmapLevel>(preview);
            if (customPreview)
            {
                return FromMapPath((*customPreview)->customLevelPath);
            }
            else
            {
                return std::nullopt;
            }
        }

        static inline std::optional<ExtraSongData> FromMapPath(std::string songPath)
        {
            if (fileexists(songPath + "/Info.dat")) {
                songPath += "/Info.dat";
            }
            else if (fileexists(songPath + "/info.dat")) {
                songPath += "/info.dat";
            }
            else {
                // If we can't find the info.dat file, we can't parse it.
                // So we return nullopt.
                return std::nullopt;
            }
            rapidjson::Document document;
            document.Parse(readfile(songPath));
            if (document.HasParseError() || !document.IsObject()) {
                return std::nullopt;
            }
            return ExtraSongData(document);
        }

        ExtraSongData(rapidjson::Document& document);
    };
}