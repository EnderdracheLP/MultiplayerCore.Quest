#include "Patchers/PlayerCountPatcher.hpp"

DEFINE_TYPE(MultiplayerCore::Patchers, PlayerCountPatcher);

namespace MultiplayerCore::Patchers {
    PlayerCountPatcher* PlayerCountPatcher::instance;
    PlayerCountPatcher* PlayerCountPatcher::get_instance() { return instance; }

    void PlayerCountPatcher::ctor(GlobalNamespace::INetworkConfig* networkConfig) {
        INVOKE_CTOR();
        _networkConfig = networkConfig;
        minPlayers = 2;

        instance = this;
    }

    void PlayerCountPatcher::Dispose() { if (instance == this) instance = nullptr; }

    bool PlayerCountPatcher::get_AddEmptyPlayerSlotForEvenCount() { return addEmptyPlayerSlotForEvenCount; }
    void PlayerCountPatcher::set_AddEmptyPlayerSlotForEvenCount(bool value) { addEmptyPlayerSlotForEvenCount = value; }

    int PlayerCountPatcher::get_MaxPlayers() { return _networkConfig->get_maxPartySize(); }
    int PlayerCountPatcher::get_MinPlayers() { return minPlayers; }
    void PlayerCountPatcher::set_MinPlayers(int value) { minPlayers = value; }
}
