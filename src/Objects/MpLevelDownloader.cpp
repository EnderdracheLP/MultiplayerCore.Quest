#include "Objects/MpLevelDownloader.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"
#include "songcore/shared/SongLoader/RuntimeSongLoader.hpp"
#include "songcore/shared/SongCore.hpp"
#include "logging.hpp"
#include "Utilities.hpp"

#include "lapiz/shared/utilities/MainThreadScheduler.hpp"
#include <chrono>

DEFINE_TYPE(MultiplayerCore::Objects, MpLevelDownloader);

namespace MultiplayerCore::Objects {
    void MpLevelDownloader::ctor() {
        INVOKE_CTOR();
    }

    std::shared_future<bool> MpLevelDownloader::TryDownloadLevelAsync(std::string levelId, std::function<void(double)> progress) {
        std::shared_future<bool> fut;
        auto dl = downloads.find(levelId);
        std::future_status status = std::future_status::timeout;
        if (dl != downloads.end()) {
            fut = dl->second;
            status = fut.wait_for(std::chrono::microseconds(0));
            if (status != std::future_status::ready)
                DEBUG("Download still in progress: {}", levelId);
            else if (status == std::future_status::ready && fut.get())
                DEBUG("Download already finished: {}", levelId);
        }

        if (dl == downloads.end() || (status == std::future_status::ready && !fut.get())) {
            DEBUG("Starting download: {}", levelId);
            downloads[levelId] = fut = il2cpp_utils::il2cpp_async(std::launch::async, std::bind(&MpLevelDownloader::TryDownloadLevelInternal, this, levelId, progress));
        }

        return fut;
    }

    bool MpLevelDownloader::TryDownloadLevelInternal(std::string levelId, std::function<void(double)> progress) {
        auto hash = HashFromLevelID(std::string_view(levelId));
        if (hash.empty()) {
            ERROR("Could not parse hash from id {}", levelId);
            return false;
        }

        auto bm = BeatSaver::API::GetBeatmapByHash(hash);
        if (bm.has_value()) {
            auto& versions = bm->GetVersions();
            auto v = std::find_if(versions.begin(), versions.end(), [hash = std::string_view(hash)](auto& x){ return x.GetHash() == hash; });
            if (v == versions.end()) return false;
            bool downloaded = false;
            BeatSaver::API::DownloadBeatmapAsync(bm.value(), *v, [&downloaded](bool){
                downloaded = true;
            });
            while(!downloaded) std::this_thread::sleep_for(std::chrono::milliseconds(50));
            SongCore::API::Loading::RefreshSongs(false).wait();

            auto level = SongCore::API::Loading::GetLevelByHash(hash);
            DEBUG("Got level: {}", fmt::ptr(level));
            return level != nullptr;
        } else {
            ERROR("Couldn't get beatmap by hash: {}", hash);
            return false;
        }
    }
}
