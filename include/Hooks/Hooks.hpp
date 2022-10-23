#pragma once
#include "GlobalFields.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "GlobalNamespace/LobbyGameStateController.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"

#include "songdownloader/shared/BeatSaverAPI.hpp"

#include "songloader/shared/API.hpp"

namespace MultiplayerCore {

    constexpr bool IsCustomLevel(const std::string& levelId) {
        return levelId.starts_with(RuntimeSongLoader::API::GetCustomLevelsPrefix());
    }

    extern bool HasSong(const std::string& levelId);

    extern StringW getModdedStateStr();
    extern StringW getMEStateStr();
    extern StringW getNEStateStr();
    extern StringW getChromaStateStr();

    extern GlobalNamespace::LobbyGameStateController* lobbyGameStateController;

    extern std::unordered_map<std::string, std::map<std::string, int>> entitlementDictionary;

    extern GlobalNamespace::IPreviewBeatmapLevel* loadingPreviewBeatmapLevel;
    extern GlobalNamespace::BeatmapDifficulty loadingBeatmapDifficulty;
    extern GlobalNamespace::BeatmapCharacteristicSO* loadingBeatmapCharacteristic;
    extern GlobalNamespace::IDifficultyBeatmap* loadingDifficultyBeatmap;
    extern GlobalNamespace::GameplayModifiers* loadingGameplayModifiers;

    class Hooks {
        static void NetworkplayerEntitlementChecker();
        static void MaxPlayerHooks();
        static void EnvironmentHooks();
        static void QuickplayHooks();
        static void Early_SessionManagerAndExtendedPlayerHooks();
        static void Late_SessionManagerAndExtendedPlayerHooks();
        static void MultiplayerStatusHook();
        static void MultiplayerUnavailableReasonHook();

    public:
        static const void InstallEarlyHooks() {
            Early_SessionManagerAndExtendedPlayerHooks();
            MultiplayerStatusHook();
        }

        static const void InstallLateHooks() {
            NetworkplayerEntitlementChecker();
            MaxPlayerHooks();
            EnvironmentHooks();
            QuickplayHooks();
            Late_SessionManagerAndExtendedPlayerHooks();
            MultiplayerUnavailableReasonHook();
        }
    };
}