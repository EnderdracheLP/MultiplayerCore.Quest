#include "Beatmaps/NetworkBeatmapLevel.hpp"
#include "Beatmaps/BeatSaverPreviewMediaData.hpp"
#include "lapiz/shared/utilities/MainThreadScheduler.hpp"
#include "bsml/shared/Helpers/utilities.hpp"
#include "logging.hpp"
#include "tasks.hpp"

#include "songdownloader/shared/BeatSaverAPI.hpp"
#include <thread>

DEFINE_TYPE(MultiplayerCore::Beatmaps, NetworkBeatmapLevel);

namespace MultiplayerCore::Beatmaps {
    void NetworkBeatmapLevel::ctor_1(Packets::MpBeatmapPacket* packet) {
        INVOKE_CTOR();
        INVOKE_BASE_CTOR(classof(Abstractions::MpBeatmapLevel*));

        _ctor(
            false,
            fmt::format("custom_level_{}", packet->levelHash),
            packet->songName,
            packet->songSubName,
            packet->songAuthorName,
            ArrayW<StringW>({ StringW(packet->levelAuthorName) }),
            ArrayW<StringW>::Empty(),
            packet->beatsPerMinute,
            -6.0f,
            0,
            0,
            0,
            packet->songDuration,
            GlobalNamespace::PlayerSensitivityFlag::Unknown,
            BeatSaverPreviewMediaData::New_ctor(packet->levelHash)->i_IPreviewMediaData(),
            nullptr
        );

        // TODO: Check if we need requirements and other extra data here
        requirements.emplace(packet->characteristic, packet->requirements);
        difficultyColors.emplace(packet->characteristic, packet->mapColors);
        for (const auto& contributor : packet->contributors)
            contributors.emplace_back(contributor);

        _packet = packet;
		set_levelHash(static_cast<std::string>(_packet->levelHash));
    }
}
