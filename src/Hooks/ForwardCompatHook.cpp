#include "hooking.hpp"
#include "logging.hpp"

#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "LiteNetLib/Utils/NetDataWriter.hpp"
#include "LiteNetLib/Utils/NetDataReader.hpp"

MAKE_AUTO_HOOK_MATCH(LevelCompletionResults_Serialize, &::GlobalNamespace::LevelCompletionResults::Serialize, void, GlobalNamespace::LevelCompletionResults* self, LiteNetLib::Utils::NetDataWriter* writer) {
    LevelCompletionResults_Serialize(self, writer);
    writer->Put(false); // Invalidated always false
}

MAKE_AUTO_HOOK_MATCH(LevelCompletionResults_CreateFromSerializedData, &::GlobalNamespace::LevelCompletionResults::CreateFromSerializedData, GlobalNamespace::LevelCompletionResults*, LiteNetLib::Utils::NetDataReader* reader) {
    auto result = LevelCompletionResults_CreateFromSerializedData(reader);
    try {
        reader->GetBool(); // Invalidated always false
    } catch (const std::exception& e) {
        WARNING("LevelCompletionResults_CreateFromSerializedData GetBool failed, player on outdated version: {}", e.what());
    }
    return result;
}