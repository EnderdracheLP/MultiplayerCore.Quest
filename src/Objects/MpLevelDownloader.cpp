#include "Objects/MpLevelDownloader.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"
#include "Utilities.hpp"
#include "logging.hpp"

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
            downloads[levelId] = fut = std::async(std::launch::async, std::bind(&MpLevelDownloader::TryDownloadLevelInternal, this, levelId, progress));
        }

        return fut;
    }

    bool MpLevelDownloader::TryDownloadLevelInternal(std::string levelId, std::function<void(double)> progress) {
        auto hash = Utilities::HashForLevelId(levelId);
        if (hash.empty()) {
            ERROR("Could not parse hash from id {}", levelId);
            return false;
        }

        auto bm = BeatSaver::API::GetBeatmapByHash(hash);
        if (bm.has_value()) {
            // TODO: this is all sketch as fuck, check if this even makes sense
            bool done, result;
            auto onFinished =
                [&done, &result](bool finished){
                    result = finished;
                    done = true;
                };

            if (progress) {
                BeatSaver::API::DownloadBeatmapAsync(bm.value(), onFinished, [progress](auto p){ progress(p);});
            } else {
                BeatSaver::API::DownloadBeatmapAsync(bm.value(), onFinished);
            }

            while(!done) {
                std::this_thread::yield();
            }

            // TODO: should we also wait for songloader to be done reloading songs? check that

            return result;
        } else {
            ERROR("Couldn't get beatmap by hash: {}", hash);
            return false;
        }
    }
}
