#include "main.hpp"
#include "Models/MpStatusData.hpp"
#include "GlobalNamespace/MultiplayerStatusData_UserMessage.hpp"
#include "GlobalNamespace/MultiplayerStatusData_UserMessage_LocalizedMessage.hpp"
// #include "Newtonsoft/Json/JsonConvert.hpp"
using namespace GlobalNamespace;

DEFINE_TYPE(MultiplayerCore::Models, MpStatusData);

namespace MultiplayerCore::Models {
    void MpStatusData::New(StringW json) {
        rapidjson::Document document;
        document.Parse(static_cast<std::string>(json));
        if (!document.HasParseError() && document.IsObject()) {
            if (document.HasMember("requiredMods")) {
                auto& requiredMods = document["requiredMods"];
                if (requiredMods.IsArray()) {
                    for (auto& requiredMod : requiredMods.GetArray()) {
                        if (requiredMod.IsObject() && requiredMod.HasMember("id") && requiredMod.HasMember("version")) {
                            auto& id = requiredMod["id"];
                            auto& version = requiredMod["version"];
                            if (id.IsString() && version.IsString()) {
                                this->requiredMods.push_back({id.GetString(), version.GetString()});
                            }
                        }
                    }
                }
            }
            if (document.HasMember("minimumAppVersion")) {
                auto& minimumAppVersion = document["minimumAppVersion"];
                if (minimumAppVersion.IsString()) {
                    this->minimumAppVersion = minimumAppVersion.GetString();
                }
            }
            if (document.HasMember("status")) {
                auto& status = document["status"];
                if (status.IsInt()) {
                    this->status = static_cast<MultiplayerStatusData::AvailabilityStatus>(status.GetInt());
                }
            }
            if (document.HasMember("maintenanceStartTime")) {
                auto& maintenanceStartTime = document["maintenanceStartTime"];
                if (maintenanceStartTime.IsInt64()) {
                    this->maintenanceStartTime = maintenanceStartTime.GetInt64();
                }
            }
            if (document.HasMember("maintenanceEndTime")) {
                auto& maintenanceEndTime = document["maintenanceEndTime"];
                if (maintenanceEndTime.IsInt64()) {
                    this->maintenanceEndTime = maintenanceEndTime.GetInt64();
                }
            }
            if (document.HasMember("useGamelift")) {
                auto& useGamelift = document["useGamelift"];
                if (useGamelift.IsBool()) {
                    this->useGamelift = useGamelift.GetBool();
                }
            }
            if (document.HasMember("userMessage")) {
                auto& userMessage = document["userMessage"];
                if (userMessage.IsObject()) {
                    if (userMessage.HasMember("localizedMessages") && userMessage["localizedMessages"].IsArray()) {
                        auto localizedMessages = userMessage["localizedMessages"].GetArray();
                        this->userMessage = GlobalNamespace::MultiplayerStatusData::UserMessage::New_ctor();
                        this->userMessage->localizations = ArrayW<GlobalNamespace::MultiplayerStatusData::UserMessage::LocalizedMessage*>(localizedMessages.Size());
                        for (int i = 0; i < localizedMessages.Size(); i++) {
                            auto& localizedMessage = localizedMessages[i];
                            if (localizedMessage.IsObject() && localizedMessage.HasMember("language") && localizedMessage.HasMember("message")) {
                                auto& language = localizedMessage["language"];
                                auto& message = localizedMessage["message"];
                                if (language.IsString() && message.IsString() && this->userMessage) {
                                    this->userMessage->localizations[i] = GlobalNamespace::MultiplayerStatusData::UserMessage::LocalizedMessage::New_ctor();
                                    this->userMessage->localizations[i]->language = language.GetString();
                                    this->userMessage->localizations[i]->message = message.GetString();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}