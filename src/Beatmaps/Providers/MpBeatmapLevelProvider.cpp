#include "Beatmaps/Providers/MpBeatmapLevelProvider.hpp"
#include "Beatmaps/BeatSaverPreviewMediaData.hpp"
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
    MpBeatmapLevelProvider* MpBeatmapLevelProvider::_instance;

    void MpBeatmapLevelProvider::ctor() {
        INVOKE_CTOR();
        _instance = this;
        _hashToNetworkLevels = HashToLevelDict::New_ctor();
        _hashToBeatsaverLevels = HashToLevelDict::New_ctor();
    }

    void MpBeatmapLevelProvider::Dispose() {
        if (_instance == this) _instance = nullptr;
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
        if (_hashToBeatsaverLevels->TryGetValue(levelHash, byref(level))) {
            return level;
        }

        auto beatmap = BeatSaver::API::GetBeatmapByHash(static_cast<std::string>(levelHash));
        if (beatmap.has_value()) {
            level = BeatSaverBeatmapLevel::Make(levelHash, beatmap.value());
            _hashToBeatsaverLevels->Add(levelHash, level);
            return level;
        }

        return nullptr;
    }

    GlobalNamespace::BeatmapLevel* MpBeatmapLevelProvider::GetBeatmapFromLocalBeatmaps(const std::string& levelHash) {
        auto customLevel = SongCore::API::Loading::GetLevelByHash(levelHash);
        if (!customLevel) return nullptr;
        return customLevel;
    }

    GlobalNamespace::BeatmapLevel* MpBeatmapLevelProvider::GetBeatmapFromPacket(Packets::MpBeatmapPacket* packet) {
        // cache network levels
        GlobalNamespace::BeatmapLevel* level = nullptr;
        if (_hashToNetworkLevels->TryGetValue(packet->levelHash, byref(level))) {
            return level;
        }

        level = GlobalNamespace::BeatmapLevel::New_ctor(
            false,
            fmt::format("custom_level_{}", packet->levelHash),
            packet->songName,
            packet->songSubName,
            packet->songAuthorName,
            { packet->levelAuthorName },
            ArrayW<StringW>::Empty(),
            packet->beatsPerMinute,
            -6.0f,
            0,
            0,
            0,
            packet->songDuration,
            ::GlobalNamespace::PlayerSensitivityFlag::Safe,
            BeatSaverPreviewMediaData::New_ctor(packet->levelHash)->i_IPreviewMediaData(),
            nullptr
        );

        _hashToNetworkLevels->Add(packet->levelHash, level);
        return level;
    }
}
