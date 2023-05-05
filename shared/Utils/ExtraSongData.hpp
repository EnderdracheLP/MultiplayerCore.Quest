#pragma once

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "UnityEngine/Color.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "LiteNetLib/Utils/NetDataReader.hpp"
#include "LiteNetLib/Utils/NetDataWriter.hpp"

#include <optional>
#include <vector>

#include "songloader/shared/API.hpp"

#define GET_COL(col) \
    auto col##Itr = json.FindMember(#col); \
    if (col##Itr != mEnd) col = col##Itr->value.GetFloat();

namespace MultiplayerCore::Utils {
    struct ExtraSongData {
        struct Contributor {
            Contributor(const std::string& name, const std::string& role, const std::string& iconPath) :
                name(name), role(role), iconPath(iconPath) {}

            Contributor(const Contributor&) = default;
            const std::string name, role, iconPath;
        };

        struct RequirementData {
            RequirementData(std::vector<std::string>& requirements, std::vector<std::string>& suggestions, std::vector<std::string>& warnings, std::vector<std::string>& information) :
                requirements(std::move(requirements)), suggestions(std::move(suggestions)), warnings(std::move(warnings)), information(std::move(information)) {}
			const std::vector<std::string> requirements;
			const std::vector<std::string> suggestions;
			const std::vector<std::string> warnings;
			const std::vector<std::string> information;
        };

        struct MapColor : UnityEngine::Color {
            constexpr MapColor() : UnityEngine::Color(1.0f, 1.0f, 1.0f, 1.0f) {}

            constexpr MapColor(float r, float g, float b) : UnityEngine::Color(r, g, b, 1.0f) {}

            inline MapColor(const rapidjson::Value& json) : UnityEngine::Color(1.0f, 1.0f, 1.0f, 1.0f) {
                auto mEnd = json.MemberEnd();
                GET_COL(r); GET_COL(g); GET_COL(b);
            }

            inline MapColor(LiteNetLib::Utils::NetDataReader* reader)
                : UnityEngine::Color(reader->GetFloat(), reader->GetFloat(), reader->GetFloat(), 1.0f) {}

            inline void Serialize(LiteNetLib::Utils::NetDataWriter* writer) const {
                writer->Put(r); writer->Put(g); writer->Put(b);
            }

            inline void Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
                r = reader->GetFloat();
                g = reader->GetFloat();
                b = reader->GetFloat();
                a = 1.0f;
            }
        };
        static_assert(sizeof(MapColor) == sizeof(UnityEngine::Color), "Size of MapColor and UnityEngine color did not match!");

        struct DifficultyData {
            DifficultyData() {}

            DifficultyData(
                std::string_view beatmapCharacteristicName,
                GlobalNamespace::BeatmapDifficulty difficulty,
                std::optional<std::string> difficultyLabel,
                std::optional<RequirementData> additionalDifficultyData,
                MapColor colorLeft,
                MapColor colorRight,
                MapColor envColorLeft,
                MapColor envColorRight,
                MapColor envColorLeftBoost,
                MapColor envColorRightBoost,
                MapColor obstacleColor
            ) :
                beatmapCharacteristicName(beatmapCharacteristicName),
                difficultyLabel(difficultyLabel),
                difficulty(difficulty),
                additionalDifficultyData(additionalDifficultyData),
                colorLeft(colorLeft),
                colorRight(colorRight),
                envColorLeft(envColorLeft),
                envColorRight(envColorRight),
                envColorLeftBoost(envColorLeftBoost),
                envColorRightBoost(envColorRightBoost),
                obstacleColor(obstacleColor) {}

            std::string beatmapCharacteristicName;
			std::optional<std::string> difficultyLabel;

			GlobalNamespace::BeatmapDifficulty difficulty;
			std::optional<RequirementData> additionalDifficultyData;

			MapColor colorLeft;
			MapColor colorRight;
			MapColor envColorLeft;
			MapColor envColorRight;
			MapColor envColorLeftBoost;
			MapColor envColorRightBoost;
			MapColor obstacleColor;
        };

        std::vector<const Contributor> contributors;
        std::string customEnvironmentName;
        std::string customEnvironmentHash;
        std::vector<const DifficultyData> difficulties;
        std::string defaultCharacteristicName;

        static inline std::optional<ExtraSongData> FromLevelHash(const std::string& levelId) {
            auto customPreview = RuntimeSongLoader::API::GetLevelByHash(levelId).value_or(nullptr);
            if (!customPreview) return std::nullopt;
            return FromMapPath(customPreview->get_customLevelPath());
        }

        static inline std::optional<ExtraSongData> FromLevelId(const std::string& levelId) {
            auto customPreview = RuntimeSongLoader::API::GetLevelById(levelId).value_or(nullptr);
            if (!customPreview) return std::nullopt;
            return FromMapPath(customPreview->get_customLevelPath());
        }

        static inline std::optional<ExtraSongData> FromPreviewBeatmapLevel(GlobalNamespace::IPreviewBeatmapLevel* preview) {
            auto customPreview = il2cpp_utils::try_cast<GlobalNamespace::CustomPreviewBeatmapLevel>(preview).value_or(nullptr);
            if (!customPreview) return std::nullopt;
            return FromMapPath(customPreview->get_customLevelPath());
        }

        static inline std::optional<ExtraSongData> FromMapPath(const std::string& path) {
            auto infoPath = path + "/Info.dat";
            if (!fileexists(infoPath)) {
                infoPath = path + "/info.dat";
                if (!fileexists(infoPath)) {
                    // couldn't find info.dat, return nullopt
                    return std::nullopt;
                }
            }

            rapidjson::Document doc;
            doc.Parse(readfile(infoPath));
            if (doc.HasParseError() || !doc.IsObject()) {
                // couldn't parse info.dat, return nullopt
                return std::nullopt;
            }

            return ExtraSongData(doc);
        }

        ExtraSongData(const rapidjson::Document& doc);
    };
}
DEFINE_IL2CPP_ARG_TYPE(::MultiplayerCore::Utils::ExtraSongData::MapColor, "UnityEngine", "Color");

#undef GET_COL
