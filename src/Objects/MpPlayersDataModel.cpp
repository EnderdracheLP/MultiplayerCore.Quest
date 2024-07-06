#include "Objects/MpPlayersDataModel.hpp"
#include "Beatmaps/Abstractions/MpBeatmapLevel.hpp"
#include "Beatmaps/Packets/MpBeatmapPacket.hpp"
#include "logging.hpp"
#include "Utilities.hpp"

#include "System/Collections/Generic/Dictionary_2.hpp"
#include "GlobalNamespace/BeatmapCharacteristicCollectionSO.hpp"
#include "GlobalNamespace/BeatmapCharacteristicCollection.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/LobbyPlayerData.hpp"
#include "GlobalNamespace/IMultiplayerSessionManager.hpp"
#include "GlobalNamespace/BeatmapLevelsModel.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"

DEFINE_TYPE(MultiplayerCore::Objects, MpPlayersDataModel);

using namespace MultiplayerCore::Beatmaps::Packets;

namespace MultiplayerCore::Objects {
    MpPlayersDataModel* MpPlayersDataModel::_instance = nullptr;

    void MpPlayersDataModel::ctor(Networking::MpPacketSerializer* packetSerializer, Beatmaps::Providers::MpBeatmapLevelProvider* beatmapLevelProvider) {
        INVOKE_CTOR();
        Base::_ctor();

        _instance = this;
        _packetSerializer = packetSerializer;
        _beatmapLevelProvider = beatmapLevelProvider;
        _lastPlayerBeatmapPackets = PacketDict::New_ctor();
    }

    void MpPlayersDataModel::Activate_override() {
        DEBUG("Activate");
        DEBUG("characteristicCollection: {}", fmt::ptr(_beatmapCharacteristicCollection));
        _packetSerializer->RegisterCallback<MpBeatmapPacket*>(std::bind(&MpPlayersDataModel::HandleMpCoreBeatmapPacket, this, std::placeholders::_1, std::placeholders::_2));
        Base::Activate();
    }

    void MpPlayersDataModel::Deactivate_override() {
        DEBUG("Deactivate");
        _packetSerializer->UnregisterCallback<MpBeatmapPacket*>();
        Base::Deactivate();
    }

    void MpPlayersDataModel::Dispose() {
        if (_instance == this) _instance = nullptr;
        Deactivate();
    }

    void MpPlayersDataModel::HandleMpCoreBeatmapPacket(MpBeatmapPacket* packet, GlobalNamespace::IConnectedPlayer* player) {
        DEBUG("'{}' selected song '{}'", player->get_userId(), packet->levelHash);
        DEBUG("Looking for characteristic '{}' on characteristicCollection: {}", packet->characteristic, fmt::ptr(_beatmapCharacteristicCollection));
        auto ch = _beatmapCharacteristicCollection->GetBeatmapCharacteristicBySerializedName(packet->characteristic);
        DEBUG("Found characteristic: {}", fmt::ptr(ch.unsafePtr()));
        auto beatmapKey = GlobalNamespace::BeatmapKey(ch, packet->difficulty, fmt::format("custom_level_{}", packet->levelHash));

        PutPlayerPacket(player->userId, packet);
        SetPlayerBeatmapLevel(player->userId, beatmapKey);
    }

    void MpPlayersDataModel::HandleMenuRpcManagerGetRecommendedBeatmap_override(StringW userId) {
        GlobalNamespace::LobbyPlayerData* localPlayerData = nullptr;
        if (_playersData->TryGetValue(userId, byref(localPlayerData)) && localPlayerData) {
            std::thread(&MpPlayersDataModel::SendMpBeatmapPacket, this, localPlayerData->beatmapKey).detach();
        }

        Base::HandleMenuRpcManagerGetRecommendedBeatmap(userId);
    }

    void MpPlayersDataModel::HandleMenuRpcManagerRecommendBeatmap_override(StringW userId, GlobalNamespace::BeatmapKeyNetSerializable* beatmapKey) {
        auto hash = HashFromLevelID(beatmapKey->levelID);
        if (!hash.empty()) return; // if this is a custom level return

        Base::HandleMenuRpcManagerRecommendBeatmap(userId, beatmapKey);
    }

    void MpPlayersDataModel::SetPlayerBeatmapLevel_override(StringW userId, GlobalNamespace::BeatmapKey& beatmapKey) {
        if (userId == _multiplayerSessionManager->localPlayer->userId) {
            std::thread(&MpPlayersDataModel::SendMpBeatmapPacket, this, static_cast<GlobalNamespace::BeatmapKey>(beatmapKey)).detach();
        }

        Base::SetPlayerBeatmapLevel(userId, byref(beatmapKey));
    }

    void MpPlayersDataModel::SendMpBeatmapPacket(GlobalNamespace::BeatmapKey beatmapKey) {
        DEBUG("Sending mp beatmap packet for level '{}'", beatmapKey.levelId);

        auto levelId = beatmapKey.levelId;
        auto hash = HashFromLevelID(levelId);
        if (hash.empty()) {
            DEBUG("not a custom level, returning...");
            return;
        }

        DEBUG("Getting level async");
        std::shared_future fut = _beatmapLevelProvider->GetBeatmapAsync(hash);
        BSML::MainThreadScheduler::AwaitFuture(fut, [fut, beatmapKey, packetSerializer = this->_packetSerializer]() {
            auto level = fut.get();
            if (!level) {
                DEBUG("couldn't get level, returning...");
                return;
            }

            DEBUG("Got level, creating packet to send");
            auto packet = MpBeatmapPacket::New_1(level, beatmapKey);
            packetSerializer->Send(packet);
        });
    }

    Beatmaps::Packets::MpBeatmapPacket* MpPlayersDataModel::GetPlayerPacket(StringW playerId) {
        Beatmaps::Packets::MpBeatmapPacket* value = nullptr;
        _lastPlayerBeatmapPackets->TryGetValue(playerId, byref(value));
        return value;
    }

    void MpPlayersDataModel::PutPlayerPacket(StringW playerId, Beatmaps::Packets::MpBeatmapPacket* packet) {
        _lastPlayerBeatmapPackets->set_Item(playerId, packet);
    }

    Beatmaps::Packets::MpBeatmapPacket* MpPlayersDataModel::FindLevelPacket(std::string_view levelHash) {
        Beatmaps::Packets::MpBeatmapPacket* packet = nullptr;
        auto enumerator = _lastPlayerBeatmapPackets->GetEnumerator();
        while (enumerator.MoveNext()) {
            auto [_, p] = enumerator.Current;
            std::string packetHash(p->levelHash);
            DEBUG("Found packetHash {}", packetHash);
            std::transform(packetHash.begin(), packetHash.end(), packetHash.begin(), toupper);
            if (packetHash == levelHash) {
                packet = p;
                break;
            }
        }
        enumerator.Dispose();
        return packet;
    }
}
