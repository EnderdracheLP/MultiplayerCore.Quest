#include "Beatmaps/Providers/MpBeatmapLevelProvider.hpp"
#include "Beatmaps/NetworkBeatmapLevel.hpp"
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
    void MpBeatmapLevelProvider::ctor() {
        INVOKE_CTOR();
        _hashToNetworkLevels = HashToLevelDict::New_ctor();
        _hashToBeatsaverLevels = HashToLevelDict::New_ctor();
    }

    std::future<GlobalNamespace::BeatmapLevel*> MpBeatmapLevelProvider::GetBeatmapAsync(const std::string& levelHash) {
        auto map = GetBeatmapFromLocalBeatmaps(levelHash);
        if (map) return finished_future(map);
        return GetBeatmapFromBeatSaverAsync(levelHash);
    }

    std::future<GlobalNamespace::BeatmapLevel*> MpBeatmapLevelProvider::GetBeatmapFromBeatSaverAsync(const std::string& levelHash) {
        return il2cpp_utils::il2cpp_async(std::launch::async, std::bind(&MpBeatmapLevelProvider::GetBeatmapFromBeatSaver, this, levelHash));
    }

    GlobalNamespace::BeatmapLevel* MpBeatmapLevelProvider::GetBeatmapFromBeatSaver(std::string levelHash) {
        GlobalNamespace::BeatmapLevel* level = nullptr;
        if (_hashToNetworkLevels->TryGetValue(levelHash, byref(level))) {
            return level;
        }

        auto beatmap = BeatSaver::API::GetBeatmapByHash(static_cast<std::string>(levelHash));
        if (beatmap.has_value()) {
            level = BeatSaverBeatmapLevel::Make(levelHash, beatmap.value());
            // Somehow it can happen that the level is already in the cache at this point, despiste us checking before
            // TODO: Check if that can still happen
            if (!_hashToNetworkLevels->ContainsKey(levelHash)) _hashToNetworkLevels->Add(levelHash, level);
            return level;
        }

        return nullptr;
    }

    GlobalNamespace::BeatmapLevel* MpBeatmapLevelProvider::GetBeatmapFromLocalBeatmaps(const std::string& levelHash) {
        auto customLevel = SongCore::API::Loading::GetLevelByHash(levelHash);
        if (!customLevel) return nullptr;
        return Beatmaps::LocalBeatmapLevel::New_ctor(levelHash, customLevel);
    }

    GlobalNamespace::BeatmapLevel* MpBeatmapLevelProvider::GetBeatmapFromPacket(Packets::MpBeatmapPacket* packet) {
        // cache network levels
        GlobalNamespace::BeatmapLevel* level = nullptr;
        if (_hashToNetworkLevels->TryGetValue(packet->levelHash, byref(level))) {
            return level;
        }

        level = NetworkBeatmapLevel::New_ctor(packet);
        _hashToNetworkLevels->Add(packet->levelHash, level);
        return level;
    }
}
