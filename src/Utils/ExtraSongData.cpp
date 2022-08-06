#include "main.hpp"
#include "Utils/ExtraSongData.hpp"

namespace MultiplayerCore::Utils {
    ExtraSongData::ExtraSongData(rapidjson::Document& document)
    {
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
                            GlobalNamespace::BeatmapDifficulty diffDifficulty = GlobalNamespace::BeatmapDifficulty::Normal;
                            if (diffItr != dB.MemberEnd() && diffItr->value.IsString()) {
                                auto diffEnumName = diffItr->value.GetString();
                                diffDifficulty = EnumUtils::GetEnumValue<GlobalNamespace::BeatmapDifficulty>(diffEnumName);
                            }
                            // GlobalNamespace::BeatmapDifficulty diffDifficulty = (diffItr != dB.MemberEnd() && diffItr->value.IsInt()) ? diffItr->value.GetInt() : GlobalNamespace::BeatmapDifficulty::Normal;

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
}