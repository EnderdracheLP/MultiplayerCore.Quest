#include "Players/MpPlayerManager.hpp"
#include "bsml/shared/Helpers/delegates.hpp"

#include "bsml/shared/BSML/SharedCoroutineStarter.hpp"
#include "System/Threading/Tasks/Task_1.hpp"

DEFINE_TYPE(MultiplayerCore::Players, MpPlayerManager);

namespace MultiplayerCore::Players {
    void MpPlayerManager::ctor(MultiplayerCore::Networking::MpPacketSerializer* packetSerializer, GlobalNamespace::IPlatformUserModel* platformUserModel, GlobalNamespace::IMultiplayerSessionManager* sessionManager) {
        INVOKE_CTOR();

        _playerData = ConcurrentPlayerDataDictionary::New_ctor();

        _packetSerializer = packetSerializer;
        _platformUserModel = platformUserModel;
        _sessionManager = sessionManager;

        BSML::SharedCoroutineStarter::StartCoroutine(custom_types::Helpers::CoroutineHelper::New(AwaitUser()));
    }

    void MpPlayerManager::Initialize() {
        _sessionManager->SetLocalPlayerState("modded", true);
        _packetSerializer->RegisterCallback<MpPlayerData*>(
            std::bind(&MpPlayerManager::HandlePlayerData, this, std::placeholders::_1, std::placeholders::_2)
        );
        _sessionManager->add_playerConnectedEvent(
            BSML::MakeSystemAction<GlobalNamespace::IConnectedPlayer*>(
                std::function<void(GlobalNamespace::IConnectedPlayer*)>(
                    std::bind(&MpPlayerManager::HandlePlayerConnected, this, std::placeholders::_1)
                )
            )
        );
    }

    custom_types::Helpers::Coroutine MpPlayerManager::AwaitUser() {
        auto t = _platformUserModel->GetUserInfo(::System::Threading::CancellationToken::get_None());
        while (!t->get_IsCompleted())
            co_yield nullptr;

        _localPlayerInfo = t->get_Result();
        co_return;
    }

    void MpPlayerManager::Dispose() {
        _packetSerializer->UnregisterCallback<MpPlayerData*>();
    }

    void MpPlayerManager::HandlePlayerConnected(GlobalNamespace::IConnectedPlayer* player) {
        if (!_localPlayerInfo) throw std::runtime_error("local player info was not yet set! make sure it is set before anything else happens!");

        auto packet = MpPlayerData::New_ctor();
        packet->platformId = _localPlayerInfo->platformUserId;
        // we're either in testing, so unknown, or we're regular user on OculusQuest or Pico
        // packet->platform = _localPlayerInfo->platform == GlobalNamespace::UserInfo::Platform::Test ? MultiplayerCore::Players::Platform::Unknown : MultiplayerCore::Players::Platform::OculusQuest;
        if (_localPlayerInfo->platform == GlobalNamespace::UserInfo::Platform::Test) {
            packet->platform = MultiplayerCore::Players::Platform::Unknown;
        } else if (_localPlayerInfo->platform.value__ == 20) {
            packet->platform = MultiplayerCore::Players::Platform::Pico;
        }
        else if (_localPlayerInfo->platform == GlobalNamespace::UserInfo::Platform::Oculus) {
            packet->platform = MultiplayerCore::Players::Platform::OculusQuest;
        } else {
            packet->platform = MultiplayerCore::Players::Platform::Unknown;
        }
        _packetSerializer->Send(packet);
    }

    void MpPlayerManager::HandlePlayerData(MpPlayerData* packet, GlobalNamespace::IConnectedPlayer* player) {
        _playerData->set_Item(player->get_userId(), packet);
        PlayerConnectedEvent.invoke(player, packet);
    }

    bool MpPlayerManager::TryGetPlayer(StringW userId, MpPlayerData*& outplayer) {
        if (_playerData->ContainsKey(userId)) {
            outplayer = _playerData->get_Item(userId);
            return true;
        }
        return false;
    }

    MpPlayerData* MpPlayerManager::GetPlayer(StringW userId) {
        MpPlayerData* player = nullptr;
        TryGetPlayer(userId, player);
        return player;
    }
}
