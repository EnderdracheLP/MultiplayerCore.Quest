#pragma once

#include "custom-types/shared/macros.hpp"
#include "System/Threading/CancellationToken.hpp"
#include <functional>
#include <future>
#include <unordered_map>

// TODO: check if this can just be done using songdownloader instead? maybe wrapping SongDownloader with this is more convenient?

DECLARE_CLASS_CODEGEN(MultiplayerCore::Objects, MpLevelDownloader, Il2CppObject,
    DECLARE_CTOR(ctor);
    public:
        std::shared_future<bool> TryDownloadLevelAsync(std::string levelId, std::function<void(double)> progress = nullptr);
    private:
        std::unordered_map<std::string, std::shared_future<bool>> downloads;
        bool TryDownloadLevelInternal(std::string levelId, std::function<void(double)> progress = nullptr);
)
