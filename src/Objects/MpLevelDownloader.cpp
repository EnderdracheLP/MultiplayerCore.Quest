#include "Objects/MpLevelDownloader.hpp"
#include "beatsaverplusplus/shared/BeatSaver.hpp"
#include "songcore/shared/SongLoader/RuntimeSongLoader.hpp"
#include "songcore/shared/SongCore.hpp"
#include "logging.hpp"
#include "Utilities.hpp"

#include "lapiz/shared/utilities/MainThreadScheduler.hpp"
#include <chrono>
#include <cctype>      // std::tolower
#include <algorithm>   // std::equal

DEFINE_TYPE(MultiplayerCore::Objects, MpLevelDownloader);

namespace MultiplayerCore::Objects {
    void MpLevelDownloader::ctor() {
        INVOKE_CTOR();

        SongCore::API::Loading::GetSongWillBeDeletedEvent() += [this](::SongCore::SongLoader::CustomBeatmapLevel* level){
            auto dl = downloads.find(level->levelID);
            if (dl != downloads.end()) {
                DEBUG("Removing download future for level: {}", level->levelID);
                downloads.erase(dl);
            }
        };
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
        DEBUG("Called TryDownloadLevelInternal: {}", levelId);
        auto hash = HashFromLevelID(std::string_view(levelId));
        if (hash.empty()) {
            ERROR("Could not parse hash from id {}", levelId);
            return false;
        }

        auto bmRes = BeatSaver::API::GetBeatmapByHash(hash);
        if (bmRes.DataParsedSuccessful()) {
            const auto& versions = bmRes.responseData->GetVersions();
            const auto& v = std::find_if(versions.begin(), versions.end(),
                [hash = std::string_view(hash)](const auto& bv){ return bv.GetHash().size() == hash.size() && std::ranges::equal(bv.GetHash(), hash,
                    [](char a, char b){
                        return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
                    });
                }
            );
            if (v == versions.end()) {
                ERROR("Could not find version for hash {}", hash);
                return false;
            }
            std::optional<std::string> downloadPath;
            DEBUG("Starting download of beatmap: {}", hash);
            v->DownloadBeatmapAsync(*bmRes.responseData, [&downloadPath](const std::optional<std::filesystem::path>& path){
                downloadPath = path.has_value() ? path->string() : std::string();
            }, progress);

            while(!downloadPath.has_value()) std::this_thread::sleep_for(std::chrono::milliseconds(50));
            if (downloadPath.value().empty()) {
                WARNING("Download failed for {}", bmRes.responseData->GetId());
                return false;
            }

            DEBUG("Download finished succesfully to '{}', refreshing songs now", downloadPath.value());
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
