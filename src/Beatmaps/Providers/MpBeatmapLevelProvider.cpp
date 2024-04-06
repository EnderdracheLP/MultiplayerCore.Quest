#include "Beatmaps/Providers/MpBeatmapLevelProvider.hpp"
#include "Beatmaps/NetworkBeatmapLevel.hpp"
#include "Beatmaps/NoInfoBeatmapLevel.hpp"
#include "Beatmaps/LocalBeatmapLevel.hpp"
#include "Beatmaps/BeatSaverBeatmapLevel.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "songcore/shared/SongCore.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"

DEFINE_TYPE(MultiplayerCore::Beatmaps::Providers, MpBeatmapLevelProvider);

template<typename T>
std::future<T> finished_future(T& value) {
    std::promise<T> p;
    p.set_value(std::forward<T>(value));
    return p.get_future();
}

namespace MultiplayerCore::Beatmaps::Providers {
    std::future<GlobalNamespace::BeatmapLevel*> MpBeatmapLevelProvider::GetBeatmapAsync(const std::string& levelHash) {
        auto map = GetBeatmapFromLocalBeatmaps(levelHash);
        if (map) return finished_future(map);
        return GetBeatmapFromBeatSaverAsync(levelHash);
    }

    std::future<GlobalNamespace::BeatmapLevel*> MpBeatmapLevelProvider::GetBeatmapFromBeatSaverAsync(const std::string& levelHash) {
        return il2cpp_utils::il2cpp_async(std::launch::async, std::bind(&MpBeatmapLevelProvider::GetBeatmapFromBeatSaver, this, levelHash));
    }

    GlobalNamespace::BeatmapLevel* MpBeatmapLevelProvider::GetBeatmapFromBeatSaver(std::string levelHash) {
        auto beatmap = BeatSaver::API::GetBeatmapByHash(static_cast<std::string>(levelHash));
        if (beatmap.has_value()) {
            return BeatSaverBeatmapLevel::Make(levelHash, beatmap.value());
        }
        return nullptr;
    }

    GlobalNamespace::BeatmapLevel* MpBeatmapLevelProvider::GetBeatmapFromLocalBeatmaps(const std::string& levelHash) {
        auto customLevel = SongCore::API::Loading::GetLevelByHash(levelHash);
        if (!customLevel) return nullptr;
        return customLevel;
    }

    GlobalNamespace::BeatmapLevel* MpBeatmapLevelProvider::GetBeatmapFromPacket(Packets::MpBeatmapPacket* packet) {
        return NetworkBeatmapLevel::New_ctor(packet);
    }
}
