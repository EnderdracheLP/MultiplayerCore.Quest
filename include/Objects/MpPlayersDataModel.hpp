#pragma once

#include "custom-types/shared/macros.hpp"
#include "GlobalNamespace/LobbyPlayersDataModel.hpp"
#include "GlobalNamespace/BeatmapKeyNetSerializable.hpp"
#include "System/IDisposable.hpp"

#include "Networking/MpPacketSerializer.hpp"
#include "Beatmaps/Providers/MpBeatmapLevelProvider.hpp"
#include "Beatmaps/Packets/MpBeatmapPacket.hpp"

DECLARE_CLASS_CODEGEN(MultiplayerCore::Objects, MpPlayersDataModel, GlobalNamespace::LobbyPlayersDataModel,
    DECLARE_INSTANCE_FIELD_PRIVATE(Networking::MpPacketSerializer*, _packetSerializer);
    DECLARE_INSTANCE_FIELD_PRIVATE(Beatmaps::Providers::MpBeatmapLevelProvider*, _beatmapLevelProvider);
    using PacketDict = System::Collections::Generic::Dictionary_2<StringW, Beatmaps::Packets::MpBeatmapPacket*>;
    DECLARE_INSTANCE_FIELD_PRIVATE(PacketDict*, _lastPlayerBeatmapPackets);

    DECLARE_INSTANCE_METHOD(void, HandleMpCoreBeatmapPacket, Beatmaps::Packets::MpBeatmapPacket* packet, GlobalNamespace::IConnectedPlayer* player);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &System::IDisposable::Dispose);
    DECLARE_CTOR(ctor, Networking::MpPacketSerializer* packetSerializer, Beatmaps::Providers::MpBeatmapLevelProvider* beatmapLevelProvider);

    public:
        Beatmaps::Packets::MpBeatmapPacket* GetPlayerPacket(StringW playerId);
        Beatmaps::Packets::MpBeatmapPacket* FindLevelPacket(std::string_view levelHash);

        __declspec(property(get=GetPlayerPacket, put=PutPlayerPacket)) Beatmaps::Packets::MpBeatmapPacket* PlayerPacket[];

        using Base = GlobalNamespace::LobbyPlayersDataModel;
        void Activate_override();
        void Deactivate_override();
        void HandleMenuRpcManagerGetRecommendedBeatmap_override(StringW userId);
        void HandleMenuRpcManagerRecommendBeatmap_override(StringW userId, GlobalNamespace::BeatmapKeyNetSerializable* beatmapKey);
        void SetPlayerBeatmapLevel_override(StringW userId, GlobalNamespace::BeatmapKey& beatmapKey);
        void SendMpBeatmapPacket(GlobalNamespace::BeatmapKey beatmapKey);
    private:
        void PutPlayerPacket(StringW playerId, Beatmaps::Packets::MpBeatmapPacket* packet);
)
