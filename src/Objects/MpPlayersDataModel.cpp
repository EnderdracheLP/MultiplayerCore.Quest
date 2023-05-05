#include "Objects/MpPlayersDataModel.hpp"
#include "Beatmaps/Abstractions/MpBeatmapLevel.hpp"
#include "logging.hpp"
#include "Utilities.hpp"

#include "System/Collections/Generic/Dictionary_2.hpp"
#include "GlobalNamespace/BeatmapCharacteristicCollectionSO.hpp"
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
        auto ch = beatmapCharacteristicCollection->GetBeatmapCharacteristicBySerializedName(packet->characteristic);
        auto preview = _beatmapLevelProvider->GetBeatmapFromPacket(packet);
        SetPlayerBeatmapLevel(player->get_userId(), GlobalNamespace::PreviewDifficultyBeatmap::New_ctor(preview, ch, packet->difficulty));
    }

    void MpPlayersDataModel::HandleMenuRpcManagerGetRecommendedBeatmap_override(StringW userId) {
        auto localPlayerData = playersData->get_Item(userId);
        // TODO: null checking
        std::string levelId(localPlayerData->get_beatmapLevel()->get_beatmapLevel()->get_levelID());
        if (levelId.empty()) return;
        auto hash = Utilities::HashForLevelId(levelId);
        if (!hash.empty())
            _packetSerializer->Send(MpBeatmapPacket::New_1(localPlayerData->get_beatmapLevel()));
        GlobalNamespace::LobbyPlayersDataModel::HandleMenuRpcManagerGetRecommendedBeatmap(userId);
    }

    void MpPlayersDataModel::HandleMenuRpcManagerRecommendBeatmap_override(StringW userId, GlobalNamespace::BeatmapIdentifierNetSerializable* beatmapId) {
        auto hash = Utilities::HashForLevelId(beatmapId->get_levelID());
        if (!hash.empty()) return;

        GlobalNamespace::LobbyPlayersDataModel::HandleMenuRpcManagerRecommendBeatmap(userId, beatmapId);
    }

    void MpPlayersDataModel::SetLocalPlayerBeatmapLevel_override(GlobalNamespace::PreviewDifficultyBeatmap* beatmapLevel) {
        auto id = beatmapLevel->get_beatmapLevel()->get_levelID();
        DEBUG("Local player selected song '{}'", id);
        auto hash = Utilities::HashForLevelId(id);
        if (!hash.empty()) {
            auto isMpBeatmapLevel = il2cpp_utils::try_cast<Beatmaps::Abstractions::MpBeatmapLevel*>(beatmapLevel->get_beatmapLevel()).has_value();
            if (!isMpBeatmapLevel)
                beatmapLevel->set_beatmapLevel(_beatmapLevelProvider->GetBeatmapAsync(hash).get());
            _packetSerializer->Send(MpBeatmapPacket::New_1(beatmapLevel));
        }
        GlobalNamespace::LobbyPlayersDataModel::SetLocalPlayerBeatmapLevel(beatmapLevel);
    }
}
