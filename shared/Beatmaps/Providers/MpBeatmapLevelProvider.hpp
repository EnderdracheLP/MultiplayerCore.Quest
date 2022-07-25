#pragma once
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "../Packets/MpBeatmapPacket.hpp"


namespace MultiplayerCore::Beatmaps::Providers {
    struct MpBeatmapLevelProvider {
        /// <summary>
        /// Gets an <see cref="GlobalNamespace::IPreviewBeatmapLevel*"/> for the specified level hash.
        /// </summary>
        /// <param name="levelHash">The hash of the level to get</param>
        /// <returns>An <see cref="GlobalNamespace::IPreviewBeatmapLevel*"/> with a matching level hash</returns>
        static GlobalNamespace::IPreviewBeatmapLevel* GetBeatmap(StringW levelHash);

        /// <summary>
        /// Gets an <see cref="GlobalNamespace::IPreviewBeatmapLevel*"/> for the specified level hash from local, already downloaded beatmaps.
        /// </summary>
        /// <param name="levelHash">The hash of the level to get</param>
        /// <returns>An <see cref="GlobalNamespace::IPreviewBeatmapLevel*"/> with a matching level hash</returns>
        static GlobalNamespace::IPreviewBeatmapLevel* GetBeatmapFromLocalBeatmaps(StringW levelHash);

        /// <summary>
        /// Gets an <see cref="GlobalNamespace::IPreviewBeatmapLevel*"/> for the specified level hash from BeatSaver.
        /// </summary>
        /// <param name="levelHash">The hash of the level to get</param>
        /// <returns>An <see cref="GlobalNamespace::IPreviewBeatmapLevel*"/> with a matching level hash, or null if none was found.</returns>
        static GlobalNamespace::IPreviewBeatmapLevel* GetBeatmapFromBeatSaver(StringW levelHash);
        //{
        //    Beatmap ? beatmap = await _beatsaver.BeatmapByHash(levelHash);
        //    if (beatmap == null)
        //        return null;
        //    return new BeatSaverBeatmapLevel(levelHash, beatmap);
        //}

        /// <summary>
        /// Gets an <see cref="GlobalNamespace::IPreviewBeatmapLevel*"/> from the information in the provided packet.
        /// </summary>
        /// <param name="packet">The packet to get preview data from</param>
        /// <returns>An <see cref="GlobalNamespace::IPreviewBeatmapLevel*"/> with a cover from BeatSaver.</returns>
        static GlobalNamespace::IPreviewBeatmapLevel* GetBeatmapFromPacket(MultiplayerCore::Beatmaps::Packets::MpBeatmapPacket* packet);
            //= > new NetworkBeatmapLevel(packet, _beatsaver);
    };
}