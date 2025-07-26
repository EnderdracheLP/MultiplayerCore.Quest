#include "hooking.hpp"
#include "logging.hpp"
#include "tasks.hpp"

#include "System/Threading/Tasks/Task_1.hpp"
#include "GlobalNamespace/NetworkUtility.hpp"
#include "GlobalNamespace/AuthenticationToken.hpp"
#include "GlobalNamespace/PlatformAuthenticationTokenProvider.hpp"

#include "GlobalNamespace/OculusPlatformUserModel.hpp"
#include "GlobalNamespace/UserInfo.hpp"

#include "Oculus/Platform/Request.hpp"
#include "Oculus/Platform/Message.hpp"
#include "Oculus/Platform/Callback.hpp"
#include "Oculus/Platform/Entitlements.hpp"
#include "Oculus/Platform/Models/Error.hpp"
#include "Oculus/Platform/CAPI.hpp"

#include "custom-types/shared/delegate.hpp"

#include <atomic>
#include <exception>
#include <sys/system_properties.h>

static ConstString dummy_auth("who_is_rem_?");

static std::string PicoBrand = "pico";
static std::atomic_bool isPico;
static std::atomic_bool checkedForPico;

MAKE_AUTO_HOOK_ORIG_MATCH(PlatformAuthenticationTokenProvider_GetAuthenticationToken, &GlobalNamespace::PlatformAuthenticationTokenProvider::GetAuthenticationToken, 
System::Threading::Tasks::Task_1<GlobalNamespace::AuthenticationToken>*, GlobalNamespace::PlatformAuthenticationTokenProvider* self) {
    if (isPico.load()) self->_platform = GlobalNamespace::AuthenticationToken::Platform(20); // Set platform to 20 for Pico

    auto t = PlatformAuthenticationTokenProvider_GetAuthenticationToken(self);
    return MultiplayerCore::StartTask<GlobalNamespace::AuthenticationToken>([=](){
        using namespace std::chrono_literals;
        while (!(t->IsCompleted || t->IsCanceled)) std::this_thread::sleep_for(50ms);
        GlobalNamespace::AuthenticationToken token = t->ResultOnSuccess;
        bool hasToken = !System::String::IsNullOrEmpty(token.sessionToken);
        if (token && hasToken) {
            INFO("Successfully got auth token, returning it!");
            if (token.platform == GlobalNamespace::AuthenticationToken::Platform::Oculus) 
                token.platform = GlobalNamespace::AuthenticationToken::Platform::OculusQuest; // Makes sure platform is set to OculusQuest
            return token;
        }
        else if (!hasToken && self->_platform.value__ != 20) ERROR("Session token is null or empty!!! Either they don't own the game or modded games no longer have access to tokens");
        WARNING("Failed to get auth token, returning custom authentication token!");

        // Check users entitlement to the app
        bool userEntitled = true;
        if (self->_platform.value__ != 20)
        {
            bool entitltiementCheckRan = false;
            INFO("Checking entitlement to the app");
            // auto request = Oculus::Platform::Entitlements::IsUserEntitledToApplication();
            auto request = Oculus::Platform::Request::New_ctor(
                Oculus::Platform::CAPI::ovr_Entitlement_GetIsViewerEntitled());
            request->OnComplete(custom_types::MakeDelegate<Oculus::Platform::Message::Callback*>(
                std::function<void(Oculus::Platform::Message*)>([&](Oculus::Platform::Message* msg) {
                    if (msg->IsError) {
                        ERROR("User is not entitled to the app: {}", msg->GetError()->Message);
                        userEntitled = false;
                    }
                    else {
                        INFO("User is entitled to the app, returning custom authentication token!");
                        // userEntitled = true;
                    }
                    entitltiementCheckRan = true;
                }
            )));

            while (!entitltiementCheckRan) std::this_thread::sleep_for(50ms);
        }

        return GlobalNamespace::AuthenticationToken(
            self->_platform == GlobalNamespace::AuthenticationToken::Platform::Oculus ? GlobalNamespace::AuthenticationToken::Platform::OculusQuest : self->_platform,
            self->_userId, // Important for server and client to keep track of players, should not be messed with if possible
            self->_userName,
            userEntitled ? dummy_auth : nullptr
        );
    });
}




MAKE_AUTO_HOOK_ORIG_MATCH(OculusPlatformUserModel_GetUserInfo, &GlobalNamespace::OculusPlatformUserModel::GetUserInfo, System::Threading::Tasks::Task_1<GlobalNamespace::UserInfo*>*, GlobalNamespace::OculusPlatformUserModel* self, System::Threading::CancellationToken ctx) {;
    if (!checkedForPico.load()) {
        auto prop = __system_property_find("ro.product.brand");
        if (prop)
            __system_property_read_callback(prop, [](void* cookie, const char* name, const char* value, uint32_t serial) {
                std::string brand(value);
                INFO("Device brand: {}", brand);
                // std::string brand_lower;
                // brand_lower.resize(brand.size());
                // std::transform(brand.begin(), brand.end(), brand_lower.begin(),
                //             [](unsigned char c){ return std::tolower(c); });
                // isPico.store(brand_lower.find(PicoBrand) != std::string::npos);
                isPico.store(std::search(brand.begin(), brand.end(), PicoBrand.begin(), PicoBrand.end(), [](char a, char b){
                    return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
                }) != brand.end());
                DEBUG("Brand Check result: {}", isPico.load());
                checkedForPico.store(true);
            }, nullptr);
    }

    auto t = OculusPlatformUserModel_GetUserInfo(self, ctx);
    if (isPico.load()) {
        DEBUG("Got Orig task, on Pico, starting custom task");
        try {
            return MultiplayerCore::StartTask<GlobalNamespace::UserInfo*>([=](){
                using namespace std::chrono_literals;
                DEBUG("Start UserInfoTask");
                while (!(t->IsCompleted || t->IsCanceled)) std::this_thread::sleep_for(50ms);
                GlobalNamespace::UserInfo* info;
                if (!t->IsFaulted) {
                    info = t->Result;
                } else ERROR("UserInfo Task faulted");
                if (info) {
                    // INFO("Successfully got user info, returning it!");
                    // if (isPico.load())
                    // {
                    INFO("User is on Pico, changing platform to 20");
                    info->platform = GlobalNamespace::UserInfo::Platform(20);
                    // }
                } else {
                    ERROR("UserInfo null!");
                }
                return info;
            });
        } catch (const std::exception& ex) {
            ERROR("Could not Start Task: {}", ex.what());
            return t;
        }
    }
    DEBUG("On Oculus, skipping custom task, returning orig");
    return t;
    // t->ContinueWith(custom_types::MakeDelegate<::System::Threading::Tasks::Task_1<GlobalNamespace::UserInfo*>>())
}


