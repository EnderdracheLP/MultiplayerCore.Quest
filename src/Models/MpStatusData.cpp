#include "Models/MpStatusData.hpp"

#include "logging.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "GlobalNamespace/MultiplayerStatusData.hpp"

DEFINE_TYPE(MultiplayerCore::Models, MpStatusData);

template<typename T>
std::optional<T> Get(rapidjson::Value const& value, std::string const& name);

template<>
std::optional<bool> Get<bool>(rapidjson::Value const& value, std::string const& name) {
    auto itr = value.FindMember(name);
    if (itr != value.MemberEnd() && itr->value.IsBool()) return itr->value.GetBool();
    return {};
}

template<>
std::optional<std::string> Get<std::string>(rapidjson::Value const& value, std::string const& name) {
    auto itr = value.FindMember(name);
    if (itr != value.MemberEnd() && itr->value.IsString()) return std::string(itr->value.GetString(), itr->value.GetStringLength());
    return {};
}

template<>
std::optional<StringW> Get<StringW>(rapidjson::Value const& value, std::string const& name) {
    auto itr = value.FindMember(name);
    if (itr != value.MemberEnd() && itr->value.IsString()) return std::string(itr->value.GetString(), itr->value.GetStringLength());
    return {};
}

template<>
std::optional<int> Get<int>(rapidjson::Value const& value, std::string const& name) {
    auto itr = value.FindMember(name);
    if (itr != value.MemberEnd() && itr->value.IsInt()) return itr->value.GetInt();
    return {};
}

template<>
std::optional<int64_t> Get<int64_t>(rapidjson::Value const& value, std::string const& name) {
    auto itr = value.FindMember(name);
    if (itr != value.MemberEnd() && itr->value.IsInt64()) return itr->value.GetInt64();
    return {};
}

template<>
std::optional<GlobalNamespace::MultiplayerStatusData::AvailabilityStatus> Get<GlobalNamespace::MultiplayerStatusData::AvailabilityStatus>(rapidjson::Value const& value, std::string const& name) {
    auto itr = value.FindMember(name);
    if (itr != value.MemberEnd() && itr->value.IsInt()) return itr->value.GetInt();
    return {};
}

template<>
std::optional<std::vector<MultiplayerCore::Models::MpStatusData::RequiredMod>> Get<std::vector<MultiplayerCore::Models::MpStatusData::RequiredMod>>(rapidjson::Value const& value, std::string const& name) {
    auto itr = value.FindMember(name);
    if (itr == value.MemberEnd() || !itr->value.IsArray()) return std::nullopt;

    std::vector<MultiplayerCore::Models::MpStatusData::RequiredMod> ret;
    for (const auto& requiredMod : itr->value.GetArray()) {
        if (!requiredMod.IsObject()) continue;

        auto const memberEnd = requiredMod.MemberEnd();
        auto const idItr = requiredMod.FindMember("id");
        auto const versionItr = requiredMod.FindMember("version");
        auto const requiredItr = requiredMod.FindMember("required");

        if (idItr != memberEnd && idItr->value.IsString() &&
            versionItr != memberEnd && versionItr->value.IsString() &&
            requiredItr != memberEnd && requiredItr->value.IsBool()) {
                std::string_view const idStr(idItr->value.GetString(), idItr->value.GetStringLength());
                auto itr = idStr.find(".Quest");
                if (itr != std::string::npos) {
                    ret.emplace_back(
                        std::string(idStr.substr(0, itr)),
                        versionItr->value.Get<std::string>(),
                        requiredItr->value.GetBool()
                    );
                }
        }
    }

    return ret;
}

#define GET(identifier, v) \
if (auto parsed = Get<std::decay_t<decltype(v)>>(doc, #identifier); parsed.has_value()) { \
    v = std::move(*parsed); \
}

namespace MultiplayerCore::Models {
    void MpStatusData::New(StringW json) {
        // cpp ctor
        INVOKE_CTOR();
        // base ctor
        GlobalNamespace::MultiplayerStatusData::_ctor();

        std::string js(json);
        DEBUG("Json: {}", js);
        rapidjson::Document doc;
        doc.Parse(js);

        if (doc.HasParseError() || !doc.IsObject()) return;

        auto docEnd = doc.MemberEnd();
        GET(required_mods, _requiredMods);

        GET(minimum_app_version, minimumAppVersion);
        GET(maximum_app_version, maximumAppVersion);

        // simple get from json into values
        GET(status, status);
        GET(maintenanceStartTime, maintenanceStartTime);
        GET(maintenanceEndTime, maintenanceEndTime);
        GET(useGamelift, useGamelift);
        GET(use_ssl, _useSsl);
        GET(name, _name);
        GET(description, _description);
        GET(image_url, _imageUrl);
        GET(max_players, _maxPlayers);
        GET(supports_pp_modifiers, _supportsPPModifiers);
        GET(supports_pp_difficulties, _supportsPPDifficulties);
        GET(supports_pp_maps, _supportsPPMaps);

        auto userMessageItr = doc.FindMember("user_message");
        if (userMessageItr != docEnd && userMessageItr->value.IsObject()) {
            const auto& localizedMessagesItr = userMessageItr->value.FindMember("localizations");
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
