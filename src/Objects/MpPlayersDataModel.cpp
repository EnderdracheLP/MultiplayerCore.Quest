#include "Objects/MpPlayersDataModel.hpp"
#include "Beatmaps/Abstractions/MpBeatmapLevel.hpp"
#include "Beatmaps/NoInfoBeatmapLevel.hpp"
#include "logging.hpp"
#include "songcore/shared/SongLoader/RuntimeSongLoader.hpp"

#include "System/Collections/Generic/Dictionary_2.hpp"
#include "GlobalNamespace/BeatmapCharacteristicCollectionSO.hpp"
#include "GlobalNamespace/BeatmapCharacteristicCollection.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/LobbyPlayerData.hpp"

DEFINE_TYPE(MultiplayerCore::Objects, MpPlayersDataModel);

using namespace MultiplayerCore::Beatmaps::Packets;

namespace MultiplayerCore::Objects {
    void MpPlayersDataModel::ctor(Networking::MpPacketSerializer* packetSerializer, Beatmaps::Providers::MpBeatmapLevelProvider* beatmapLevelProvider) {
        INVOKE_CTOR();
        INVOKE_BASE_CTOR(classof(GlobalNamespace::LobbyPlayersDataModel*));

        _packetSerializer = packetSerializer;
        _beatmapLevelProvider = beatmapLevelProvider;
    }

    void MpPlayersDataModel::Activate_override() {
        _packetSerializer->RegisterCallback<MpBeatmapPacket*>(std::bind(&MpPlayersDataModel::HandleMpexBeatmapPacket, this, std::placeholders::_1, std::placeholders::_2));
        GlobalNamespace::LobbyPlayersDataModel::Activate();
    }

    void MpPlayersDataModel::Deactivate_override() {
        _packetSerializer->UnregisterCallback<MpBeatmapPacket*>();
        GlobalNamespace::LobbyPlayersDataModel::Deactivate();
    }

    void MpPlayersDataModel::Dispose() {
        Deactivate();
    }

    void MpPlayersDataModel::HandleMpexBeatmapPacket(MpBeatmapPacket* packet, GlobalNamespace::IConnectedPlayer* player) {
        DEBUG("'{}' selected song '{}'", player->get_userId(), packet->levelHash);
        auto ch = _beatmapCharacteristicCollection->GetBeatmapCharacteristicBySerializedName(packet->characteristic);
        auto preview = _beatmapLevelProvider->GetBeatmapFromPacket(packet);
        auto beatmapKey = GlobalNamespace::BeatmapKey(ch, packet->difficulty, fmt::format("custom_level_{}", packet->levelHash));
        SetPlayerBeatmapLevel(player->get_userId(), beatmapKey);
    }

    void MpPlayersDataModel::HandleMenuRpcManagerGetRecommendedBeatmap_override(StringW userId) {
        GlobalNamespace::LobbyPlayerData* localPlayerData = nullptr;
        if (!_playersData->TryGetValue(userId, byref(localPlayerData)) || !localPlayerData) return; // if we can't get the player, just return

        auto beatmapKey = localPlayerData->beatmapKey;

        std::string levelId(beatmapKey.levelId ? beatmapKey.levelId : "");
        if (levelId.empty()) return;

        auto level = _beatmapLevelsModel->GetBeatmapLevel(levelId);
        if (level) _packetSerializer->Send(MpBeatmapPacket::New_1(level, beatmapKey));
        GlobalNamespace::LobbyPlayersDataModel::HandleMenuRpcManagerGetRecommendedBeatmap(userId);
    }

    void MpPlayersDataModel::HandleMenuRpcManagerRecommendBeatmap_override(StringW userId, GlobalNamespace::BeatmapKeyNetSerializable* beatmapKey) {
        auto hash = SongCore::SongLoader::RuntimeSongLoader::GetHashFromLevelID(beatmapKey->levelID);
        if (!hash.empty()) return;

        GlobalNamespace::LobbyPlayersDataModel::HandleMenuRpcManagerRecommendBeatmap(userId, beatmapKey);
    }

    void MpPlayersDataModel::SetLocalPlayerBeatmapLevel_override(GlobalNamespace::BeatmapKey& beatmapKey) {
        std::string id(beatmapKey.levelId);
        DEBUG("Local player selected song '{}'", id);
        auto hash = SongCore::SongLoader::RuntimeSongLoader::GetHashFromLevelID(id);
        // check whether there is a hash to get (custom level)
        if (!hash.empty()) {
            auto level = _beatmapLevelsModel->GetBeatmapLevel(beatmapKey.levelId);
            if (!level) {
                _packetSerializer->Send(MpBeatmapPacket::New_1(Beatmaps::NoInfoBeatmapLevel::New_ctor(hash), beatmapKey));
            }
        }
        GlobalNamespace::LobbyPlayersDataModel::SetLocalPlayerBeatmapLevel(beatmapKey);
    }
}
