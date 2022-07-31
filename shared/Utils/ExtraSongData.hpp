#pragma once
#include <string>
#include <string_view>
#include <optional>
#include <vector>

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/Color.hpp"

#include "LiteNetLib/Utils/NetDataReader.hpp"
#include "LiteNetLib/Utils/NetDataWriter.hpp"

#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"

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
            return ExtraSongData(songPath);
        }

        private:

        inline ExtraSongData(std::string songPath)
        {
            rapidjson::Document document;
            document.Parse(readfile(songPath));
            if (!document.HasParseError() && document.IsObject()) {
                auto cdItr = document.FindMember("_customData");
                if (cdItr != document.MemberEnd() && cdItr->value.IsObject()) {
                    auto contributorsItr = cdItr->value.FindMember("contributors");
                    if (contributorsItr != cdItr->value.MemberEnd() && contributorsItr->value.IsArray() && !contributorsItr->value.Empty()) {
                        for (auto& c : contributorsItr->value.GetArray()) {
                            auto nameItr = c.FindMember("_name");
                            auto roleItr = c.FindMember("_role");
                            auto iconItr = c.FindMember("_iconPath");
                            if (nameItr != c.MemberEnd() && nameItr->value.IsString() && 
                            roleItr != c.MemberEnd() && roleItr->value.IsString()) {
                                contributors.emplace_back(nameItr->value.GetString(), roleItr->value.GetString(), 
                                (iconItr != c.MemberEnd() && iconItr->value.IsString()) ? iconItr->value.GetString() : "");
                            }
                        }
                    }
                    auto customEnvironmentItr = cdItr->value.FindMember("_customEnvironmentName");
                    if (customEnvironmentItr != cdItr->value.MemberEnd() && customEnvironmentItr->value.IsString()) {
                        _customEnvironmentName = customEnvironmentItr->value.GetString();
                    }
                    auto customEnvironmentHashItr = cdItr->value.FindMember("_customEnvironmentHash");
                    if (customEnvironmentHashItr != cdItr->value.MemberEnd() && customEnvironmentHashItr->value.IsString()) {
                        _customEnvironmentHash = customEnvironmentHashItr->value.GetString();
                    }
                    auto defaultCharacteristicItr = cdItr->value.FindMember("_defaultCharacteristic");
                    if (defaultCharacteristicItr != cdItr->value.MemberEnd() && defaultCharacteristicItr->value.IsString()) {
                        _defaultCharacteristicName = defaultCharacteristicItr->value.GetString();
                    }
                }

                std::vector<const DifficultyData> diffData;
                auto difficultyBeatmapSetsItr = document.FindMember("_difficultyBeatmapSets");
                if (difficultyBeatmapSetsItr != document.MemberEnd() && difficultyBeatmapSetsItr->value.IsArray() && !difficultyBeatmapSetsItr->value.Empty()) {
                    for (auto& dBS : difficultyBeatmapSetsItr->value.GetArray()) 
                    {
                        rapidjson::Value::ConstMemberIterator bCN = dBS.FindMember("_beatmapCharacteristicName");
                        std::string_view beatmapCharacteristicName = bCN != dBS.MemberEnd() && bCN->value.IsString() ? bCN->value.GetString() : "";
                        auto dbItr = dBS.FindMember("_difficultyBeatmaps");
                        if (dbItr != dBS.MemberEnd() && dbItr->value.IsArray() && !dbItr->value.Empty()) {
                            for (auto& dB : dbItr->value.GetArray()) {
                                std::vector<std::string> diffRequirements;
                                std::vector<std::string> diffSuggestions;
                                std::vector<std::string> diffWarnings;
                                std::vector<std::string> diffInfo;
                                std::string diffLabel;
                                MapColor diffLeft;
                                MapColor diffRight;
                                MapColor diffEnvLeft;
                                MapColor diffEnvRight;
                                MapColor diffEnvLeftBoost;
                                MapColor diffEnvRightBoost;
                                MapColor diffObstacle;
                                auto diffItr = dB.FindMember("_difficulty");
                                GlobalNamespace::BeatmapDifficulty diffDifficulty = (diffItr != dB.MemberEnd() && diffItr->value.IsInt()) ? diffItr->value.GetInt() : GlobalNamespace::BeatmapDifficulty::Normal;

                                auto customDataItr = dB.FindMember("_customData");
                                if (customDataItr != dB.MemberEnd() && customDataItr->value.IsObject()) {
                                    
                                    auto difficultyLabelItr = customDataItr->value.FindMember("_difficultyLabel");
                                    if (difficultyLabelItr != customDataItr->value.MemberEnd() && difficultyLabelItr->value.IsString()) {
                                        diffLabel = difficultyLabelItr->value.GetString();
                                    }

                                    auto colorLeftItr = customDataItr->value.FindMember("_colorLeft");
                                    if (colorLeftItr != customDataItr->value.MemberEnd() && colorLeftItr->value.IsObject()) {
                                        diffLeft = MapColor(colorLeftItr->value.GetObject());
                                    }

                                    auto colorRightItr = customDataItr->value.FindMember("_colorRight");
                                    if (colorRightItr != customDataItr->value.MemberEnd() && colorRightItr->value.IsObject()) {
                                        diffRight = MapColor(colorRightItr->value.GetObject());
                                    }

                                    auto colorEnvLeftItr = customDataItr->value.FindMember("_colorEnvLeft");
                                    if (colorEnvLeftItr != customDataItr->value.MemberEnd() && colorEnvLeftItr->value.IsObject()) {
                                        diffEnvLeft = MapColor(colorEnvLeftItr->value.GetObject());
                                    }

                                    auto colorEnvRightItr = customDataItr->value.FindMember("_colorEnvRight");
                                    if (colorEnvRightItr != customDataItr->value.MemberEnd() && colorEnvRightItr->value.IsObject()) {
                                        diffEnvRight = MapColor(colorEnvRightItr->value.GetObject());
                                    }

                                    auto colorEnvLeftBoostItr = customDataItr->value.FindMember("_colorEnvLeftBoost");
                                    if (colorEnvLeftBoostItr != customDataItr->value.MemberEnd() && colorEnvLeftBoostItr->value.IsObject()) {
                                        diffEnvLeftBoost = MapColor(colorEnvLeftBoostItr->value.GetObject());
                                    }

                                    auto colorEnvRightBoostItr = customDataItr->value.FindMember("_colorEnvRightBoost");
                                    if (colorEnvRightBoostItr != customDataItr->value.MemberEnd() && colorEnvRightBoostItr->value.IsObject()) {
                                        diffEnvRightBoost = MapColor(colorEnvRightBoostItr->value.GetObject());
                                    }

                                    auto colorObstacleItr = customDataItr->value.FindMember("_colorObstacle");
                                    if (colorObstacleItr != customDataItr->value.MemberEnd() && colorObstacleItr->value.IsObject()) {
                                        diffObstacle = MapColor(colorObstacleItr->value.GetObject());
                                    }

                                    auto warningsItr = customDataItr->value.FindMember("_warnings");
                                    if (warningsItr != customDataItr->value.MemberEnd() && warningsItr->value.IsArray() && !warningsItr->value.Empty()) {
                                        for (auto& w : warningsItr->value.GetArray()) {
                                            if (w.IsString()) {
                                                diffWarnings.emplace_back(w.GetString());
                                            }
                                        }
                                    }

                                    auto informationItr = customDataItr->value.FindMember("_information");
                                    if (informationItr != customDataItr->value.MemberEnd() && informationItr->value.IsArray() && !informationItr->value.Empty()) {
                                        for (auto& info : informationItr->value.GetArray()) {
                                            if (info.IsString()) {
                                                diffInfo.emplace_back(info.GetString());
                                            }
                                        }
                                    }

                                    auto suggestionsItr = customDataItr->value.FindMember("_suggestions");
                                    if (suggestionsItr != customDataItr->value.MemberEnd() && suggestionsItr->value.IsArray() && !suggestionsItr->value.Empty()) {
                                        for (auto& s : suggestionsItr->value.GetArray()) {
                                            if (s.IsString()) {
                                                diffSuggestions.emplace_back(s.GetString());
                                            }
                                        }
                                    }
                                    
                                    auto requirementItr = customDataItr->value.FindMember("_requirements");
                                    if (requirementItr != customDataItr->value.MemberEnd() && requirementItr->value.IsArray() && !requirementItr->value.Empty()) {
                                        for (auto& req : requirementItr->value.GetArray()) {
                                            if (req.IsString()) {
                                                diffRequirements.emplace_back(req.GetString());
                                            }
                                        }
                                    }

                                    RequirementData difReqData = { diffRequirements, diffSuggestions, diffWarnings, diffInfo };

                                    diffData.push_back({ beatmapCharacteristicName, 
                                        diffDifficulty, diffLabel, 
                                        difReqData, 
                                        diffLeft, diffRight, 
                                        diffEnvLeft, diffEnvRight, 
                                        diffEnvLeftBoost, diffEnvRightBoost, 
                                        diffObstacle 
                                    });
                                }
                            }
                        }
                    }
                }
                _difficulties = std::move(diffData);
            }
        }
    };
}