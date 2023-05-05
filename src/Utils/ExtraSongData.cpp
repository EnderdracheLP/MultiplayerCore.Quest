#include "Utils/ExtraSongData.hpp"
#include "Utils/EnumUtils.hpp"
#include "logging.hpp"

#define GET_STR(src, id) \
    auto id##Itr = src.FindMember("_" #id); \
    if (id##Itr != src.MemberEnd() && id##Itr->value.IsString()) \
        id = std::string(id##Itr->value.GetString(), id##Itr->value.GetStringLength());

#define GET_COL(col, identifier) \
    auto col##Itr = cD.FindMember(identifier);\
    if (col##Itr != cD.MemberEnd() && col##Itr->value.IsObject()) \
        col = MapColor(col##Itr->value.GetObject())

#define GET_VEC(vec, identifier) \
    auto vec##Itr = cD.FindMember(identifier);\
    if (vec##Itr != cD.MemberEnd() && vec##Itr->value.IsArray() && !vec##Itr->value.Empty()) {\
        for (const auto& val : vec##Itr->value.GetArray()) {\
            if (val.IsString()) {\
                vec.emplace_back(val.GetString(), val.GetStringLength());\
            }\
        }\
    }
namespace MultiplayerCore::Utils {
    ExtraSongData::ExtraSongData(const rapidjson::Document& doc) {
        if (doc.HasParseError() || !doc.IsObject()) return;

        auto cdItr = doc.FindMember("_customData");
        if (cdItr != doc.MemberEnd() && cdItr->value.IsObject()) {
            auto contributorsItr = cdItr->value.FindMember("contributors");
            if (contributorsItr != cdItr->value.MemberEnd() && contributorsItr->value.IsArray() && !contributorsItr->value.Empty()) {
                for (const auto& c : contributorsItr->value.GetArray()) {
                    auto mEnd = c.MemberEnd();
                    auto nameItr = c.FindMember("_name");
                    auto roleItr = c.FindMember("_role");
                    auto iconItr = c.FindMember("_iconPath");

                    if (nameItr != mEnd && nameItr->value.IsString() &&
                        roleItr != mEnd && roleItr->value.IsString()) {
                            contributors.emplace_back(
                                std::string(nameItr->value.GetString(), nameItr->value.GetStringLength()),
                                std::string(roleItr->value.GetString(), roleItr->value.GetStringLength()),
                                (iconItr != mEnd && iconItr->value.IsString()) ? std::string(iconItr->value.GetString(), iconItr->value.GetStringLength()) : ""
                            );
                        }
                }
            }
            GET_STR(cdItr->value, customEnvironmentName);
            GET_STR(cdItr->value, customEnvironmentHash);
            GET_STR(cdItr->value, defaultCharacteristicName);
        }

        auto dBSItr = doc.FindMember("_difficultyBeatmapSets");
        if (dBSItr != doc.MemberEnd() && dBSItr->value.IsArray() && !dBSItr->value.Empty()) {
            difficulties.reserve(dBSItr->value.Size());
            for (const auto& set : dBSItr->value.GetArray()) {
                auto bCNItr = set.FindMember("_beatmapCharacteristicName");

                std::string_view beatmapCharacteristicName (
                    (bCNItr != set.MemberEnd() && bCNItr->value.IsString()) ?
                    std::string_view(bCNItr->value.GetString(), bCNItr->value.GetStringLength()) :
                    std::string_view("", 0)
                );

                auto dBItr = set.FindMember("_difficultyBeatmaps");
                if (dBItr != set.MemberEnd() && dBItr->value.IsArray() && !dBItr->value.Empty()) {

                    for (const auto& dB : dBItr->value.GetArray()) {
                        std::vector<std::string> diffRequirements, diffSuggestions, diffWarnings, diffInfo;
                        std::string diffLabel;
                        MapColor diffLeft, diffRight, diffEnvLeft, diffEnvRight, diffEnvLeftBoost, diffEnvRightBoost, diffObstacle;
                        GlobalNamespace::BeatmapDifficulty diffDifficulty = GlobalNamespace::BeatmapDifficulty::Normal;
                        auto diffItr = dB.FindMember("_difficulty");
                        if (diffItr != dB.MemberEnd() && diffItr->value.IsString()) {
                            std::string diffEnumName(diffItr->value.GetString(), diffItr->value.GetStringLength());
                            diffDifficulty = EnumUtils::GetEnumValue<GlobalNamespace::BeatmapDifficulty>(diffEnumName);
                        }

                        auto cDItr = dB.FindMember("_customData");
                        if (cDItr != dB.MemberEnd() && cDItr->value.IsObject()) {
                            const auto& cD = cDItr->value;
                            auto dLItr = cD.FindMember("_difficultyLabel");
                            if (dLItr != cD.MemberEnd() && dLItr->value.IsString()) {
                                diffLabel = std::string(dLItr->value.GetString(), dLItr->value.GetStringLength());
                            }

                            GET_COL(diffLeft, "_colorLeft");
                            GET_COL(diffRight, "_colorRight");
                            GET_COL(diffEnvLeft, "_colorEnvLeft");
                            GET_COL(diffEnvRight, "_colorEnvRight");
                            GET_COL(diffEnvLeftBoost, "_colorEnvLeftBoost");
                            GET_COL(diffEnvRightBoost, "_colorEnvRightBoost");
                            GET_COL(diffObstacle, "_colorObstacle");

                            GET_VEC(diffRequirements, "_requirements");
                            GET_VEC(diffSuggestions, "_suggestions");
                            GET_VEC(diffWarnings, "_warnings");
                            GET_VEC(diffInfo, "_information");

                            RequirementData diffReqData(
                                diffRequirements,
                                diffSuggestions,
                                diffWarnings,
                                diffInfo
                            );

                            difficulties.emplace_back(
                                beatmapCharacteristicName,
                                diffDifficulty,
                                diffLabel,
                                diffReqData,
                                diffLeft,
                                diffRight,
                                diffEnvLeft,
                                diffEnvRight,
                                diffEnvLeftBoost,
                                diffEnvRightBoost,
                                diffObstacle
                            );
                        }
                    }
                }
            }
        }
    }
}
