#include "logging.hpp"
#include "hooking.hpp"

#include "GlobalNamespace/MultiplayerLevelFinishedController.hpp"
#include "GlobalNamespace/MultiplayerLevelCompletionResults.hpp"
#include "GlobalNamespace/BeatmapBasicData.hpp"

#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Generic/InsertionBehavior.hpp"

MAKE_AUTO_HOOK_ORIG_MATCH(MultiplayerLevelFinishedController_HandleRpcLevelFinished, &GlobalNamespace::MultiplayerLevelFinishedController::HandleRpcLevelFinished, void, GlobalNamespace::MultiplayerLevelFinishedController* self, StringW userId, GlobalNamespace::MultiplayerLevelCompletionResults* results) {
    // TODO: Possibly try manually getting notes count and run the calculation ourselves, 
    // for now we'll just skip validation if notes count is 0
    if (self->_beatmapBasicData->notesCount <= 0 && results->hasAnyResults) {
                DEBUG("BeatmapData noteCount is 0, skipping validation");
				if (self->_otherPlayersCompletionResults->TryInsert(userId, results, ::System::Collections::Generic::InsertionBehavior::OverwriteExisting))
                    return;
                else {
                    WARNING("Failed to insert completion results for user {}", userId);
                }
    }
    
    MultiplayerLevelFinishedController_HandleRpcLevelFinished(self, userId, results);
}