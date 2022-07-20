#include "Beatmaps/NoInfoBeatmapLevel.hpp"
#include "custom-types/shared/register.hpp"
#include "songloader/shared/API.hpp"

DEFINE_TYPE(MultiplayerCore::Beatmaps, NoInfoBeatmapLevel);

namespace MultiplayerCore::Beatmaps {
	void NoInfoBeatmapLevel::New(StringW hash) {
		levelHash = hash;
	}

	System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* NoInfoBeatmapLevel::GetCoverImageAsync(System::Threading::CancellationToken cancellationToken) {
		return System::Threading::Tasks::Task_1<UnityEngine::Sprite*>::New_ctor(static_cast<UnityEngine::Sprite*>(nullptr));
	}


#pragma region All the pain in form of getter functions

	ConstString emptyString("");

	StringW NoInfoBeatmapLevel::get_songName() {
		return emptyString;
	}

	StringW NoInfoBeatmapLevel::get_songSubName() {
		return emptyString;
	}

	StringW NoInfoBeatmapLevel::get_songAuthorName() {
		return emptyString;
	}

	StringW NoInfoBeatmapLevel::get_levelAuthorName() {
		return emptyString;
	}

#pragma endregion
}
