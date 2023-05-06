#include "Beatmaps/NetworkBeatmapLevel.hpp"
#include "lapiz/shared/utilities/MainThreadScheduler.hpp"
#include "bsml/shared/Helpers/utilities.hpp"

#include "songdownloader/shared/BeatSaverAPI.hpp"

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
			coverImageTask = System::Threading::Tasks::Task_1<UnityEngine::Sprite*>::New_ctor(static_cast<UnityEngine::Sprite*>(nullptr));
            coverImageTask->m_stateFlags = System::Threading::Tasks::Task::TASK_STATE_STARTED;

            // TODO: turn lambdas into callbacks or something with std::bind
            BeatSaver::API::GetBeatmapByHashAsync(get_levelHash(),
                [this](auto beatmap) {
                    if (beatmap.has_value()) {
                        BeatSaver::API::GetCoverImageAsync(beatmap.value(), [this](std::vector<uint8_t> bytes){
                            Lapiz::Utilities::MainThreadScheduler::Schedule([this, bytes = il2cpp_utils::vectorToArray(bytes)] {
                                coverImageTask->TrySetResult(BSML::Utilities::LoadSpriteRaw(bytes));
                                reinterpret_cast<System::Threading::Tasks::Task*>(coverImageTask)->m_stateFlags = System::Threading::Tasks::Task::TASK_STATE_RAN_TO_COMPLETION;
                            });
                        });
                    } else {
                        coverImageTask->m_stateFlags = System::Threading::Tasks::Task::TASK_STATE_RAN_TO_COMPLETION;
                    }
                }
            );
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
