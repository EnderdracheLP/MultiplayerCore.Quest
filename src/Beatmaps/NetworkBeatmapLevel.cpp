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

            BeatSaver::API::GetBeatmapByHashAsync(get_levelHash(), std::bind(&NetworkBeatmapLevel::OnGetBeatmapComplete, this, std::placeholders::_1));
        }
        return coverImageTask;
    }

    void NetworkBeatmapLevel::OnGetBeatmapComplete(std::__ndk1::optional<BeatSaver::Beatmap> beatmapOpt) {
        if (beatmapOpt.has_value()) {
            BeatSaver::API::GetCoverImageAsync(beatmapOpt.value(), std::bind(&NetworkBeatmapLevel::OnGetCoverImageComplete, this, std::placeholders::_1));
        } else {
            coverImageTask->m_stateFlags = System::Threading::Tasks::Task::TASK_STATE_RAN_TO_COMPLETION;
        }
    }

    void NetworkBeatmapLevel::OnGetCoverImageComplete(std::vector<uint8_t> bytes) {
        Lapiz::Utilities::MainThreadScheduler::Schedule([this, bytes = il2cpp_utils::vectorToArray(bytes)] {
            coverImageTask->TrySetResult(BSML::Utilities::LoadSpriteRaw(bytes));
            coverImageTask->m_stateFlags = System::Threading::Tasks::Task::TASK_STATE_RAN_TO_COMPLETION;
        });
    }

    StringW NetworkBeatmapLevel::get_songName() { return _packet->songName; }
	StringW NetworkBeatmapLevel::get_songSubName() { return _packet->songSubName; }
	StringW NetworkBeatmapLevel::get_songAuthorName() { return _packet->songAuthorName; }
	StringW NetworkBeatmapLevel::get_levelAuthorName() { return _packet->levelAuthorName; }
	float NetworkBeatmapLevel::get_beatsPerMinute() { return _packet->beatsPerMinute; }
	float NetworkBeatmapLevel::get_songDuration() { return _packet->songDuration; }
}
