#pragma once
#include "custom-types/shared/macros.hpp"
#include "Networking/Abstractions/MpPacket.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"

DECLARE_CLASS_CODEGEN_DLL(MultiplayerCore::Beatmaps::Packets, MpBeatmapPacket, classof(MultiplayerCore::Networking::Abstractions::MpPacket*), "MultiplayerCore.Beatmaps.Packets",
    DECLARE_INSTANCE_FIELD(StringW, levelHash);

    DECLARE_INSTANCE_FIELD(StringW, songName);
    DECLARE_INSTANCE_FIELD(StringW, songSubName);
    DECLARE_INSTANCE_FIELD(StringW, songAuthorName);
    DECLARE_INSTANCE_FIELD(StringW, levelAuthorName);

    DECLARE_INSTANCE_FIELD(float, beatsPerMinute);
    DECLARE_INSTANCE_FIELD(float, songDuration);

    DECLARE_INSTANCE_FIELD(StringW, characteristic);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::BeatmapDifficulty, difficulty);

    DECLARE_CTOR(New, GlobalNamespace::IPreviewBeatmapLevel* preview, StringW characteristic, GlobalNamespace::BeatmapDifficulty difficulty);

    DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&LiteNetLib::Utils::INetSerializable::Serialize>::get(), LiteNetLib::Utils::NetDataWriter* writer);
    DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::il2cpp_type_check::MetadataGetter<&LiteNetLib::Utils::INetSerializable::Deserialize>::get(), LiteNetLib::Utils::NetDataReader* reader);
)
