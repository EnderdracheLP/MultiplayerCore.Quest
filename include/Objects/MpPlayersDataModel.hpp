#pragma once

#include "custom-types/shared/macros.hpp"
#include "GlobalNamespace/LobbyPlayersDataModel.hpp"
#include "GlobalNamespace/PreviewDifficultyBeatmap.hpp"
#include "GlobalNamespace/BeatmapIdentifierNetSerializable.hpp"
#include "System/IDisposable.hpp"

#include "Networking/MpPacketSerializer.hpp"
#include "Beatmaps/Providers/MpBeatmapLevelProvider.hpp"
#include "Beatmaps/Packets/MpBeatmapPacket.hpp"

DECLARE_CLASS_CODEGEN(MultiplayerCore::Objects, MpPlayersDataModel, GlobalNamespace::LobbyPlayersDataModel,
    DECLARE_INSTANCE_FIELD_PRIVATE(Networking::MpPacketSerializer*, _packetSerializer);
    DECLARE_INSTANCE_FIELD_PRIVATE(Beatmaps::Providers::MpBeatmapLevelProvider*, _beatmapLevelProvider);

    DECLARE_INSTANCE_METHOD(void, Activate_override);
    DECLARE_INSTANCE_METHOD(void, Deactivate_override);
    DECLARE_INSTANCE_METHOD(void, HandleMenuRpcManagerGetRecommendedBeatmap_override, StringW userId);
    DECLARE_INSTANCE_METHOD(void, HandleMenuRpcManagerRecommendBeatmap_override, StringW userId, GlobalNamespace::BeatmapIdentifierNetSerializable* beatmapId);
    DECLARE_INSTANCE_METHOD(void, SetLocalPlayerBeatmapLevel_override, GlobalNamespace::PreviewDifficultyBeatmap* beatmapLevel);

    DECLARE_INSTANCE_METHOD(void, HandleMpexBeatmapPacket, Beatmaps::Packets::MpBeatmapPacket* packet, GlobalNamespace::IConnectedPlayer* player);
    DECLARE_OVERRIDE_METHOD_MATCH(void, Dispose, &::System::IDisposable::Dispose);
    DECLARE_CTOR(ctor, Networking::MpPacketSerializer* packetSerializer, Beatmaps::Providers::MpBeatmapLevelProvider* beatmapLevelProvider);
)
