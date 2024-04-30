#include "Beatmaps/BeatSaverPreviewMediaData.hpp"
#include "tasks.hpp"

#include "UnityEngine/Networking/UnityWebRequestMultimedia.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"
#include "UnityEngine/Networking/UnityWebRequestAsyncOperation.hpp"
#include "UnityEngine/Networking/DownloadHandlerAudioClip.hpp"
#include "UnityEngine/AudioType.hpp"
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
                auto coverOpt = BeatSaver::API::GetCoverImage(_levelHash);
                if (coverOpt.has_value()) {
                    auto coverBytes = il2cpp_utils::vectorToArray(coverOpt.value());

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
        if (_cachedAudioClip && _cachedAudioClip->m_CachedPtr) {
            return Task_1<UnityEngine::AudioClip*>::New_ctor(_cachedAudioClip);
        } else {
            _cachedAudioClip = nullptr;
        }

        if (!_audioClipTask || _audioClipTask->IsCancellationRequested) {
            _audioClipTask = StartTask<UnityEngine::AudioClip*>([this](CancellationToken cancelToken) -> UnityEngine::AudioClip* {
                auto beatmapOpt = BeatSaver::API::GetBeatmapByHash(_levelHash);
                if (cancelToken.IsCancellationRequested) return (UnityEngine::AudioClip*)nullptr;

                if (beatmapOpt.has_value()) {
                    auto& versions = beatmapOpt->GetVersions();
                    auto v = std::find_if(versions.begin(), versions.end(), [levelHash = std::string_view(this->_levelHash)](auto x){ return x.GetHash() == levelHash; });
                    if (v == versions.end()) return (UnityEngine::AudioClip*)nullptr;

                    auto webRequest = UnityEngine::Networking::UnityWebRequestMultimedia::GetAudioClip(v->GetPreviewURL(), UnityEngine::AudioType::MPEG);
                    auto www = webRequest->SendWebRequest();
                    while (!www->get_isDone()) std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    if (cancelToken.IsCancellationRequested) return (UnityEngine::AudioClip*)nullptr;

                    std::atomic<UnityEngine::AudioClip*> result = nullptr;
                    BSML::MainThreadScheduler::Schedule([webRequest, &result](){
                        result = UnityEngine::Networking::DownloadHandlerAudioClip::GetContent(webRequest);
                    });
                    while (!result) std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    return (UnityEngine::AudioClip*)result;
                }

                return nullptr;
            }, std::forward<CancellationToken>(cancellationToken));
        }

        return _audioClipTask;
    }

    void BeatSaverPreviewMediaData::UnloadPreviewAudioClip() {
        if (_cachedAudioClip && _cachedAudioClip->m_CachedPtr) {
            UnityEngine::Object::Destroy(_cachedAudioClip);
        }
        _cachedAudioClip = nullptr;
        _audioClipTask = nullptr;
    }
}
