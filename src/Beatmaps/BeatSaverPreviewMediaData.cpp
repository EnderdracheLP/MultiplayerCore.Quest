#include "Beatmaps/BeatSaverPreviewMediaData.hpp"
#include "tasks.hpp"

#include "bsml/shared/Helpers/utilities.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"
#include <chrono>

DEFINE_TYPE(MultiplayerCore::Beatmaps, BeatSaverPreviewMediaData);

using namespace System::Threading;
using namespace System::Threading::Tasks;

namespace MultiplayerCore::Beatmaps {
    void BeatSaverPreviewMediaData::ctor(StringW levelHash) {
        _levelHash = static_cast<std::string>(levelHash);
    }

    Task_1<UnityEngine::Sprite*>* BeatSaverPreviewMediaData::GetCoverSpriteAsync(CancellationToken cancellationToken) {
        if (_cachedCoverImage && _cachedCoverImage->m_CachedPtr) {
            return Task_1<UnityEngine::Sprite*>::FromResult(_cachedCoverImage);
        } else {
            _cachedCoverImage = nullptr;
        }

        if (!_coverImageTask || _coverImageTask->IsCancellationRequested) {
            _coverImageTask = StartTask<UnityEngine::Sprite*>([this](CancellationToken token) -> UnityEngine::Sprite* {
                auto beatmapOpt = BeatSaver::API::GetBeatmapByHash(_levelHash);
                if (beatmapOpt.has_value()) {
                    auto cover = BeatSaver::API::GetCoverImage(beatmapOpt.value());

                    auto coverBytes = il2cpp_utils::vectorToArray(cover);
                    std::atomic<UnityEngine::Sprite*> result = nullptr;
                    BSML::MainThreadScheduler::Schedule([coverBytes, &result](){
                        result = BSML::Utilities::LoadSpriteRaw(coverBytes);
                    });

                    while(!result) std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    return (UnityEngine::Sprite*)result;
                }
                return (UnityEngine::Sprite*)nullptr;
            }, std::forward<CancellationToken>(cancellationToken));
        }
        return _coverImageTask;
    }

    Task_1<UnityEngine::AudioClip*>* BeatSaverPreviewMediaData::GetPreviewAudioClip(CancellationToken cancellationToken) {
        return StartTask<UnityEngine::AudioClip*>([](CancellationToken cancelToken) -> UnityEngine::AudioClip* {
            // TODO: preview audio somehow (look at how bss does it?)
            return nullptr;
        }, std::forward<CancellationToken>(cancellationToken));
    }

    void BeatSaverPreviewMediaData::UnloadPreviewAudioClip() {

    }
}
