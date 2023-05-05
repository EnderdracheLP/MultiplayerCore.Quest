#include "Beatmaps/Providers/MpBeatmapLevelProvider.hpp"
#include "Beatmaps/NetworkBeatmapLevel.hpp"
#include "Beatmaps/NoInfoBeatmapLevel.hpp"
#include "Beatmaps/LocalBeatmapLevel.hpp"
#include "Beatmaps/BeatSaverBeatmapLevel.hpp"

#include "songloader/shared/API.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"

DEFINE_TYPE(MultiplayerCore::Beatmaps::Providers, MpBeatmapLevelProvider);

template<typename T>
std::future<T> finished_future(T& value) {
    std::promise<T> p;
    p.set_value(std::forward<T>(value));
    return p.get_future();
}

namespace MultiplayerCore::Beatmaps::Providers {
    std::future<GlobalNamespace::IPreviewBeatmapLevel*> MpBeatmapLevelProvider::GetBeatmapAsync(const std::string& levelHash) {
        auto map = GetBeatmapFromLocalBeatmaps(levelHash);
        if (map) return finished_future(map);
        return GetBeatmapFromBeatSaverAsync(levelHash);
    }

    std::future<GlobalNamespace::IPreviewBeatmapLevel*> MpBeatmapLevelProvider::GetBeatmapFromBeatSaverAsync(const std::string& levelHash) {
        return std::async(std::launch::async, std::bind(&MpBeatmapLevelProvider::GetBeatmapFromBeatSaver, this, levelHash));
    }

    GlobalNamespace::IPreviewBeatmapLevel* MpBeatmapLevelProvider::GetBeatmapFromBeatSaver(std::string levelHash) {
        auto beatmap = BeatSaver::API::GetBeatmapByHash(static_cast<std::string>(levelHash));
        if (beatmap.has_value()) {
            return BeatSaverBeatmapLevel::Make(levelHash, beatmap.value())->i_IPreviewBeatmapLevel();
        }
        return nullptr;
    }

    GlobalNamespace::IPreviewBeatmapLevel* MpBeatmapLevelProvider::GetBeatmapFromLocalBeatmaps(const std::string& levelHash) {
        GlobalNamespace::CustomPreviewBeatmapLevel* preview = RuntimeSongLoader::API::GetLevelByHash(levelHash).value_or(nullptr);
        if (!preview) return nullptr;
        return LocalBeatmapLevel::New_ctor(levelHash, preview->i_IPreviewBeatmapLevel())->i_IPreviewBeatmapLevel();
    }

    GlobalNamespace::IPreviewBeatmapLevel* MpBeatmapLevelProvider::GetBeatmapFromPacket(Packets::MpBeatmapPacket* packet) {
        return NetworkBeatmapLevel::New_ctor(packet)->i_IPreviewBeatmapLevel();
    }
}
