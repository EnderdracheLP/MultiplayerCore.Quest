#include "../../Networking/Abstractions/MpPacket.hpp"

#include "custom-types/shared/macros.hpp"

DECLARE_CLASS_CUSTOM(MultiplayerCore::Players::Packets, GetMpPerPlayerPacket, MultiplayerCore::Networking::Abstractions::MpPacket,
    DECLARE_DEFAULT_CTOR();
)
