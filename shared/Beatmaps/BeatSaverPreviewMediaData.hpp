#pragma once

#include "custom-types/shared/macros.hpp"

#include "GlobalNamespace/IPreviewMediaData.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/AudioClip.hpp"
#include "Packets/MpBeatmapPacket.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerCore::Beatmaps, BeatSaverPreviewMediaData, System::Object, GlobalNamespace::IPreviewMediaData*) {
    DECLARE_CTOR(ctor, StringW levelHash);

    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::Sprite*, _cachedCoverImage);
    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::AudioClip*, _cachedAudioClip);

    DECLARE_INSTANCE_FIELD_PRIVATE(System::Threading::Tasks::Task_1<UnityEngine::Sprite*>*, _coverImageTask);
    DECLARE_INSTANCE_FIELD_PRIVATE(System::Threading::Tasks::Task_1<UnityEngine::AudioClip*>*, _audioClipTask);

    DECLARE_OVERRIDE_METHOD_MATCH(System::Threading::Tasks::Task_1<UnityEngine::Sprite*>*, GetCoverSpriteAsync, &GlobalNamespace::IPreviewMediaData::GetCoverSpriteAsync, System::Threading::CancellationToken cancellationToken);
    DECLARE_OVERRIDE_METHOD_MATCH(System::Threading::Tasks::Task_1<UnityEngine::AudioClip*>*, GetPreviewAudioClip, &GlobalNamespace::IPreviewMediaData::GetPreviewAudioClip, System::Threading::CancellationToken cancellationToken);
    DECLARE_OVERRIDE_METHOD_MATCH(void, UnloadPreviewAudioClip, &GlobalNamespace::IPreviewMediaData::UnloadPreviewAudioClip);

    public:
        GlobalNamespace::IPreviewMediaData* i_IPreviewMediaData() noexcept { return reinterpret_cast<GlobalNamespace::IPreviewMediaData*>(this); }
    private:
        std::string _levelHash;
};
