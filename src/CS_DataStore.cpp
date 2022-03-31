#include "main.hpp"
#include "CS_DataStore.hpp"

DEFINE_TYPE(MultiplayerCore, DataStore);

namespace MultiplayerCore {
    void DataStore::New(GlobalNamespace::ILevelGameplaySetupData* gameplaySetupData, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap) {
        loadingGameplaySetupData = gameplaySetupData;
        loadingDifficultyBeatmap = difficultyBeatmap;
    }

    DataStore* DataStore::instance;

    DataStore* DataStore::get_Instance() {
        return instance;
    }

    DataStore* DataStore::CS_Ctor(GlobalNamespace::ILevelGameplaySetupData* gameplaySetupData, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap) {
        //instance = THROW_UNLESS(il2cpp_utils::New<MultiplayerCore::DataStore*, il2cpp_utils::CreationType::Manual>(gameplaySetupData, difficultyBeatmap));
        instance = MultiplayerCore::DataStore::New_ctor<il2cpp_utils::CreationType::Manual>(gameplaySetupData, difficultyBeatmap));
        return instance;
    }

    void DataStore::Clear() {
        if (instance) {
            instance->loadingGameplaySetupData = nullptr;
            instance->loadingDifficultyBeatmap = nullptr;
        }
    }
}