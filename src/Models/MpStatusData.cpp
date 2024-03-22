#include "Models/MpStatusData.hpp"

#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "GlobalNamespace/MultiplayerStatusData.hpp"

DEFINE_TYPE(MultiplayerCore::Models, MpStatusData);

// define used to get some types from the json in less code lines
#define GET_T(identifier, t) \
    auto identifier##Itr = doc.FindMember(#identifier); \
    if (identifier##Itr != docEnd && identifier##Itr->value.Is##t()) { \
        identifier = std::decay_t<decltype(identifier)>(identifier##Itr->value.Get##t()); \
    }

namespace MultiplayerCore::Models {
    void MpStatusData::New(StringW json) {
        // cpp ctor
        INVOKE_CTOR();
        // base ctor
        INVOKE_BASE_CTOR(classof(GlobalNamespace::MultiplayerStatusData*));

        rapidjson::Document doc;
        doc.Parse(std::string(json));

        if (!doc.HasParseError() && doc.IsObject()) {
            auto docEnd = doc.MemberEnd();
            auto requiredModsItr = doc.FindMember("requiredMods");
            if (requiredModsItr != docEnd && requiredModsItr->value.IsArray()) {
                for (const auto& requiredMod : requiredModsItr->value.GetArray()) {
                    if (!requiredMod.IsObject()) continue;
                    const auto& idItr = requiredMod.FindMember("id");
                    const auto& versionItr = requiredMod.FindMember("version");

                    if (idItr != requiredMod.MemberEnd() && idItr->value.IsString() &&
                        versionItr != requiredMod.MemberEnd() && versionItr->value.IsString()) {
                            std::string idStr(idItr->value.GetString(), idItr->value.GetStringLength());
                            const auto itr = idStr.find(".Quest");
                            if (itr != std::string::npos) {
                                requiredMods.push_back(
                                    {
                                        idStr.substr(0, itr),
                                        std::string(
                                            versionItr->value.GetString(),
                                            versionItr->value.GetStringLength()
                                        ),
                                        0
                                    }
                                );
                            }
                    }
                }
            }

            auto minimumAppVersionItr = doc.FindMember("minimumAppVersion");
            if (minimumAppVersionItr != docEnd && minimumAppVersionItr->value.IsString()) {
                minimumAppVersion = std::string_view(
                    minimumAppVersionItr->value.GetString(),
                    minimumAppVersionItr->value.GetStringLength()
                );
            }

            auto maximumAppVersionItr = doc.FindMember("maximumAppVersion");
            if (maximumAppVersionItr != docEnd && maximumAppVersionItr->value.IsString()) {
                maximumAppVersion = std::string_view(
                    maximumAppVersionItr->value.GetString(),
                    maximumAppVersionItr->value.GetStringLength()
                );
            }

            // simple get from json into values
            GET_T(status, Int);
            GET_T(maintenanceStartTime, Int64);
            GET_T(maintenanceEndTime, Int64);
            GET_T(useGamelift, Bool);

            auto userMessageItr = doc.FindMember("userMessage");
            if (userMessageItr != docEnd && userMessageItr->value.IsObject()) {
                const auto& localizedMessagesItr = userMessageItr->value.FindMember("localizedMessages");
                if (localizedMessagesItr != userMessageItr->value.MemberEnd() && localizedMessagesItr->value.IsArray()) {
                    const auto& localizedMessages = localizedMessagesItr->value.GetArray();

                    userMessage = GlobalNamespace::MultiplayerStatusData::UserMessage::New_ctor();
                    using LocalizedMessage = GlobalNamespace::MultiplayerStatusData::UserMessage::LocalizedMessage;
                    auto localizations = ArrayW<LocalizedMessage*>(localizedMessages.Size());
                    userMessage->localizations = localizations;
                    for (std::size_t i= -1; const auto& localizedMessage : localizedMessages) {
                        i++;
                        localizations[i] = LocalizedMessage::New_ctor();

                        if (!localizedMessage.IsObject()) continue;
                        auto languageItr = localizedMessage.FindMember("language");
                        auto messageItr = localizedMessage.FindMember("message");

                        if (languageItr != localizedMessage.MemberEnd() && languageItr->value.IsString() &&
                            messageItr != localizedMessage.MemberEnd() && messageItr->value.IsString()) {
                                localizations[i]->language = std::string_view(
                                    languageItr->value.GetString(),
                                    languageItr->value.GetStringLength()
                                );
                                localizations[i]->message = std::string_view(
                                    messageItr->value.GetString(),
                                    messageItr->value.GetStringLength()
                                );
                            }
                    }
                }
            }
        }
    }
}
