#include "Beatmaps/NetworkBeatmapLevel.hpp"
#include "custom-types/shared/register.hpp"
#include "GlobalNamespace/VarIntExtensions.hpp"
#include "songloader/shared/API.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "Utilities.hpp"

DEFINE_TYPE(MultiplayerCore::Beatmaps, NetworkBeatmapLevel);

namespace MultiplayerCore::Beatmaps {
	void NetworkBeatmapLevel::New(Packets::MpBeatmapPacket* packet) {
		getLogger().debug("NetworkBeatmapLevel::New:  INVOKE_CTOR();");
		INVOKE_CTOR();
		// Calling base class constructor
		this->__ctor();
		_packet = packet;
		requirements[_packet->characteristic] = std::move(_packet->requirements);
		difficultyColors[_packet->characteristic] = std::move(_packet->mapColors);
		contributors = std::move(_packet->contributors);
		levelHash =  _packet->levelHash;
	}

	System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* NetworkBeatmapLevel::GetCoverImageAsync(System::Threading::CancellationToken cancellationToken) {
		if (!coverImageTask) {
			// Create a finished task with nullptr result
			coverImageTask = System::Threading::Tasks::Task_1<UnityEngine::Sprite*>::New_ctor(static_cast<UnityEngine::Sprite*>(nullptr));
			
			// Manually set the state to started so the game will wait until we pulled the data from BeatSaver
			reinterpret_cast<System::Threading::Tasks::Task*>(coverImageTask)->m_stateFlags = System::Threading::Tasks::Task::TASK_STATE_STARTED;
			getLogger().debug("Pulling coverImage from BeatSaver, hash: %s", static_cast<std::string>(_packet->levelHash).c_str());
			BeatSaver::API::GetBeatmapByHashAsync(_packet->levelHash,
				[this](std::optional<BeatSaver::Beatmap> beatmap) {
					getLogger().debug("GetBeatmapByHashAsync for coverImage finished, was beatmap found: %s", beatmap.has_value() ? "true" : "false");
					if (beatmap.has_value()) {
						getLogger().debug("Running GetCoverImageAsync");
						BeatSaver::API::GetCoverImageAsync(beatmap.value(), [this](std::vector<uint8_t> bytes) {
							QuestUI::MainThreadScheduler::Schedule([this, bytes] {
								getLogger().debug("Got coverImage from BeatSaver");
								// If we got our CoverImage set it
								coverImageTask->TrySetResult(QuestUI::BeatSaberUI::VectorToSprite(bytes));
								// TODO: Probably specify some kind of timeout, to avoid people with slow internet not being able to see the current selection
								reinterpret_cast<System::Threading::Tasks::Task*>(coverImageTask)->m_stateFlags = System::Threading::Tasks::Task::TASK_STATE_RAN_TO_COMPLETION;
								}
							);
							}
						);
					} else {
						// If we don't have any info, we'll just use our nullptr task
						reinterpret_cast<System::Threading::Tasks::Task*>(coverImageTask)->m_stateFlags = System::Threading::Tasks::Task::TASK_STATE_RAN_TO_COMPLETION;
					}
				}
			);
		}
		if (coverImageTask)
			return coverImageTask;
		return System::Threading::Tasks::Task_1<UnityEngine::Sprite*>::New_ctor(static_cast<UnityEngine::Sprite*>(nullptr));
	}


#pragma region All the pain in form of getter functions

	StringW NetworkBeatmapLevel::get_songName() {
		return _packet->songName;
	}

	StringW NetworkBeatmapLevel::get_songSubName() {
		return _packet->songSubName;
	}

	StringW NetworkBeatmapLevel::get_songAuthorName() {
		return _packet->songAuthorName;
	}

	StringW NetworkBeatmapLevel::get_levelAuthorName() {
		return _packet->levelAuthorName;
	}

	float NetworkBeatmapLevel::get_beatsPerMinute() {
		return _packet->beatsPerMinute;
	}

	float NetworkBeatmapLevel::get_songDuration() {
		return _packet->songDuration;
	}

#pragma endregion
}
