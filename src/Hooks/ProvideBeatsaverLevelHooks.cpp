#include "logging.hpp"
#include "hooking.hpp"
#include "tasks.hpp"
#include "Utilities.hpp"

#include "songdownloader/shared/BeatSaverAPI.hpp"
#include "songcore/shared/SongCore.hpp"

#include "GlobalNamespace/BeatmapLevelsModel.hpp"
#include "GlobalNamespace/LoadBeatmapLevelDataResult.hpp"

MAKE_AUTO_HOOK_MATCH(BeatmapLevelsModel_CheckBeatmapLevelDataExistsAsync, &GlobalNamespace::BeatmapLevelsModel::CheckBeatmapLevelDataExistsAsync, System::Threading::Tasks::Task_1<bool>*, GlobalNamespace::BeatmapLevelsModel* self, StringW levelID, System::Threading::CancellationToken cancelToken) {
    auto t = BeatmapLevelsModel_CheckBeatmapLevelDataExistsAsync(self, levelID, cancelToken);
    // check for levelhash -> no hash means base game, return orig
    auto levelHash = MultiplayerCore::HashFromLevelID(levelID);
    if (levelHash.empty()) return t;

    // check if locally available, if so return orig
    if (SongCore::API::Loading::GetLevelByHash(levelHash)) return t;

    // custom level, not locally available, get on beatsaver
    return MultiplayerCore::StartTask<bool>([=](){
        using namespace std::chrono_literals;
        while (!(t->IsCompleted || t->IsCanceled)) std::this_thread::sleep_for(50ms);
        // if orig says it's available, cool beans
        if (t->Result) return true;

        // check if map is on beatsaver
        auto beatmap = BeatSaver::API::GetBeatmapByHash(levelHash);
        if (beatmap.has_value()) {
            for (auto& v : beatmap->GetVersions()) {
                if (v.GetHash() == levelHash) {
                    bool downloaded = false;
                    BeatSaver::API::DownloadBeatmapAsync(beatmap.value(), v, [&downloaded](bool v){
                        downloaded = true;
                    });
                    while (!downloaded) std::this_thread::sleep_for(50ms);

                    SongCore::API::Loading::RefreshSongs().wait();

                    auto level = SongCore::API::Loading::GetLevelByHash(levelHash);
                    if (level) return true;
                    break;
                }
            }
        }
        return false;
    });
}
