#include "MultiplayerCore.hpp"
#include "System/Exception.hpp"
#include "System/Net/Http/HttpClient.hpp"
#include "System/Net/Http/HttpContent.hpp"
#include "System/Net/Http/HttpRequestMessage.hpp"
#include "System/Net/Http/HttpResponseMessage.hpp"
#include "System/Threading/CancellationToken.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Uri.hpp"
#include "hooking.hpp"
#include "logging.hpp"

#include "Utils/EnumUtils.hpp"
#include "tasks.hpp"

using namespace System::Net::Http;
using namespace System::Threading::Tasks;
using namespace System::Threading;

MAKE_AUTO_HOOK_FIND_VERBOSE(
    HttpClient_SendAsync,
    ::il2cpp_utils::FindMethod(
        ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<
            ::System::Net::Http::HttpClient *>::get(),
        "SendAsync", std::span<Il2CppClass const *const, 0>(),
        ::std::array<Il2CppType const *, 3>{
            ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_type<
                ::System::Net::Http::HttpRequestMessage *>::get(),
            ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_type<
                ::System::Net::Http::HttpCompletionOption>::get(),
            ::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_type<
                ::System::Threading::CancellationToken>::get()}),
    Task_1<HttpResponseMessage *> *, HttpClient *self,
    HttpRequestMessage *request, HttpCompletionOption completionOption,
    CancellationToken cancellationToken) {

  auto result =
      HttpClient_SendAsync(self, request, completionOption, cancellationToken);
  auto currentServer = MultiplayerCore::API::GetCurrentServer();
  if (currentServer &&
      !System::String::IsNullOrWhiteSpace(currentServer->graphUrl) &&
      (request->RequestUri->Host ==
           System::Uri::New_ctor(currentServer->graphUrl)->Host ||
       request->RequestUri->OriginalString->StartsWith(
           currentServer->graphUrl) ||
       (!System::String::IsNullOrWhiteSpace(
            currentServer->masterServerStatusUrl) &&
        request->RequestUri->OriginalString->StartsWith(
            currentServer->masterServerStatusUrl)))) {

    il2cpp_utils::il2cpp_aware_thread([result, request]() {
      try {
        while (!result->IsCompleted && !result->IsCanceled)
          std::this_thread::yield();

        auto response = result->Result;
        if (!response->IsSuccessStatusCode) {
          ERROR("An error occurred while attempting to post to the Graph API: "
                "Uri '{}' StatusCode: {}: {}",
                request->RequestUri->ToString(),
                static_cast<int>(response->StatusCode),
                MultiplayerCore::Utils::EnumUtils::GetEnumName(
                    response->StatusCode));
          auto responseContent = response->Content->ReadAsStringAsync();
          while (!responseContent->IsCompleted && !responseContent->IsCanceled)
            std::this_thread::yield();
          DEBUG("Response: {}", responseContent->Result);
        }
      } 
      catch (const il2cpp_utils::RunMethodException &ex) {
        ERROR("An error occurred while attempting to post to the Graph API: "
              "Exception Message: {}",
              ex.what());
        auto exc =
            il2cpp_utils::try_cast<System::Exception>(const_cast<Il2CppException*>(ex.ex)).value_or(nullptr);

        // DEBUG("{}", exc->ToString());
        while (exc != nullptr) {
          ERROR(" --> {}: {}", exc->GetType()->ToString(), exc->get_Message());
          exc = exc->InnerException;
        }

      // DEBUG(ex.what());
      } 
      catch (const std::exception &ex) {
      ERROR("An error occurred while attempting to post to the Graph API: "
            "Exception Message: {}",
            ex.what());
      // DEBUG(ex.what());
      }
  }).detach();
}
return result;
}
