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
    void MpPlayersDataModel::ctor(Networking::MpPacketSerializer* packetSerializer, Beatmaps::Providers::MpBeatmapLevelProvider* beatmapLevelProvider) {
        INVOKE_CTOR();
        Base::_ctor();

        _packetSerializer = packetSerializer;
        _beatmapLevelProvider = beatmapLevelProvider;
        _lastPlayerBeatmapPackets = PacketDict::New_ctor();
    }

    void MpPlayersDataModel::Activate_override() {
        DEBUG("Activate");
        _packetSerializer->RegisterCallback<MpBeatmapPacket*>(std::bind(&MpPlayersDataModel::HandleMpCoreBeatmapPacket, this, std::placeholders::_1, std::placeholders::_2));
        Base::Activate();
    }

    void MpPlayersDataModel::Deactivate_override() {
        DEBUG("Deactivate");
        _packetSerializer->UnregisterCallback<MpBeatmapPacket*>();
        Base::Deactivate();
    }

    void MpPlayersDataModel::Dispose() {
        Deactivate();
    }

    void MpPlayersDataModel::HandlePlayerConnected(GlobalNamespace::IConnectedPlayer* connectedPlayer) {
        Base::HandleMultiplayerSessionManagerPlayerConnected(connectedPlayer);
        GlobalNamespace::LobbyPlayerData* localPlayerData = nullptr;
        if (_playersData->TryGetValue(localUserId, byref(localPlayerData)) && localPlayerData) {
            std::thread(&MpPlayersDataModel::SendMpBeatmapPacket, this, localPlayerData->beatmapKey, connectedPlayer).detach();
        }
    }

    void MpPlayersDataModel::HandleMpCoreBeatmapPacket(MpBeatmapPacket* packet, GlobalNamespace::IConnectedPlayer* player) {
        DEBUG("'{}' selected song '{}'", player->get_userId(), packet->levelHash);
        DEBUG("Looking for characteristic {} on {}", packet->characteristic, fmt::ptr(_beatmapCharacteristicCollection));
        auto ch = _beatmapCharacteristicCollection->GetBeatmapCharacteristicBySerializedName(packet->characteristic);
        auto beatmapKey = GlobalNamespace::BeatmapKey(ch, packet->difficulty, fmt::format("custom_level_{}", packet->levelHash));

        PutPlayerPacket(player->userId, packet);
        SetPlayerBeatmapLevel(player->userId, beatmapKey);
    }

    void MpPlayersDataModel::HandleMenuRpcManagerGetRecommendedBeatmap_override(StringW userId) {
        GlobalNamespace::LobbyPlayerData* localPlayerData = nullptr;
        if (_playersData->TryGetValue(userId, byref(localPlayerData)) && localPlayerData) {
            std::thread(&MpPlayersDataModel::SendMpBeatmapPacket, this, localPlayerData->beatmapKey, nullptr).detach();
        }

        Base::HandleMenuRpcManagerGetRecommendedBeatmap(userId);
    }

    void MpPlayersDataModel::HandleMenuRpcManagerRecommendBeatmap_override(StringW userId, GlobalNamespace::BeatmapKeyNetSerializable* beatmapKey) {
        auto hash = HashFromLevelID(beatmapKey->levelID);
        if (!hash.empty()) return; // if this is a custom level return

        Base::HandleMenuRpcManagerRecommendBeatmap(userId, beatmapKey);
    }

    void MpPlayersDataModel::SetLocalPlayerBeatmapLevel_override(GlobalNamespace::BeatmapKey& beatmapKey) {
        DEBUG("Setting local player beatmap level id '{}'\r\nCheck difficulty '{}'\r\nCheck characteristic '{}'\r\nCheck BeatmapKey Valid '{}'", beatmapKey.levelId, (int)beatmapKey.difficulty, beatmapKey.beatmapCharacteristic ? beatmapKey.beatmapCharacteristic->serializedName : "null", beatmapKey.IsValid());
        std::thread(&MpPlayersDataModel::SendMpBeatmapPacket, this, static_cast<GlobalNamespace::BeatmapKey>(beatmapKey), nullptr).detach();

        Base::SetLocalPlayerBeatmapLevel(byref(beatmapKey));
    }

    void MpPlayersDataModel::SendMpBeatmapPacket(GlobalNamespace::BeatmapKey beatmapKey, GlobalNamespace::IConnectedPlayer* player) {
        DEBUG("Sending mp beatmap packet for level '{}'", beatmapKey.levelId);

        auto levelId = beatmapKey.levelId;
        auto hash = HashFromLevelID(levelId);
        if (hash.empty()) {
            DEBUG("not a custom level, returning...");
            return;
        }

        std::shared_future fut = _beatmapLevelProvider->GetBeatmapAsync(hash);
        BSML::MainThreadScheduler::AwaitFuture(fut, [fut, beatmapKey, hash, player, packetSerializer = this->_packetSerializer, beatmapLevelProvider = this->_beatmapLevelProvider]() mutable -> void {
            auto level = fut.get();
            if (!level) {
                DEBUG("couldn't get level, returning...");
                return;
            }

            auto mpBeatmapLevel = il2cpp_utils::try_cast<Beatmaps::Abstractions::MpBeatmapLevel>(level).value_or(nullptr);
            if (mpBeatmapLevel && mpBeatmapLevel->requirements.empty())
            {
                WARNING("Empty requirements for level '{}', using packet instead", hash);
                auto packetLevel = beatmapLevelProvider->TryGetBeatmapFromPacketHash(hash);
                if (packetLevel) {
                    level = packetLevel;
                } else {
                    // Uh oh, we need the difficulties from this level, but we don't have them
                    // We know that in this case the selected difficulty must exist, so we can at least add that one
                    ERROR("Could not determine difficulties for level, things may break or not work as expected");
                    mpBeatmapLevel->requirements[static_cast<std::string>(beatmapKey.beatmapCharacteristic->serializedName)][(uint32_t)beatmapKey.difficulty.value__] = std::list<std::string>();
                }
            }

            auto packet = MpBeatmapPacket::New_1(level, beatmapKey);
            if (player) {
                packetSerializer->SendToPlayer(packet, player);
            } else {
                packetSerializer->Send(packet);
            }
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
        return packet;
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
