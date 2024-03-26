#include "Beatmaps/NetworkBeatmapLevel.hpp"
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

        _packet = packet;
		set_levelHash(static_cast<std::string>(_packet->levelHash));
		requirements[_packet->characteristic] = std::move(_packet->requirements);
		difficultyColors[_packet->characteristic] = std::move(_packet->mapColors);
		contributors = std::move(_packet->contributors);
    }
}
