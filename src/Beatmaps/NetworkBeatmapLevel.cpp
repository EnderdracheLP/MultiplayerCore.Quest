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
		requirements[_packet->characteristic] = std::move(_packet->requirements);
		difficultyColors[_packet->characteristic] = std::move(_packet->mapColors);
		contributors = std::move(_packet->contributors);
		set_levelHash(_packet->levelHash);
    }

	System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* NetworkBeatmapLevel::GetCoverImageAsync(System::Threading::CancellationToken cancellationToken) {
        if (!coverImageTask) {
            coverImageTask = StartTask<UnityEngine::Sprite*>([this](){
                auto beatmapOpt = BeatSaver::API::GetBeatmapByHash(get_levelHash());
                if (beatmapOpt.has_value()) {
                    auto cover = BeatSaver::API::GetCoverImage(beatmapOpt.value());

                    auto coverBytes = il2cpp_utils::vectorToArray(cover);
                    std::optional<UnityEngine::Sprite*> result;
                    Lapiz::Utilities::MainThreadScheduler::Schedule([coverBytes, &result](){
                        result = BSML::Utilities::LoadSpriteRaw(coverBytes);
                    });

                    while(!result.has_value()) std::this_thread::yield();
                    return result.value();
                }
                return (UnityEngine::Sprite*)nullptr;
            }, cancellationToken);
        }
        return coverImageTask;
    }

    StringW NetworkBeatmapLevel::get_songName() { return _packet->songName; }
	StringW NetworkBeatmapLevel::get_songSubName() { return _packet->songSubName; }
	StringW NetworkBeatmapLevel::get_songAuthorName() { return _packet->songAuthorName; }
	StringW NetworkBeatmapLevel::get_levelAuthorName() { return _packet->levelAuthorName; }
	float NetworkBeatmapLevel::get_beatsPerMinute() { return _packet->beatsPerMinute; }
	float NetworkBeatmapLevel::get_songDuration() { return _packet->songDuration; }
}
