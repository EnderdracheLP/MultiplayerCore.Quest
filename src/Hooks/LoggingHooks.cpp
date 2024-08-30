#include "logging.hpp"
#include "hooking.hpp"

#include "GlobalNamespace/ConnectedPlayerManager.hpp"
#include "GlobalNamespace/IConnection.hpp"
#include "LiteNetLib/Utils/NetDataReader.hpp"
#include "LiteNetLib/DeliveryMethod.hpp"

// TODO: something to do this:
// cause idk what quest would do here
/*
MAKE_AUTO_HOOK_MATCH(ConnectedPlayerManager_HandleNetworkReceive, &::GlobalNamespace::ConnectedPlayerManager::HandleNetworkReceive, void, GlobalNamespace::ConnectedPlayerManager* self, GlobalNamespace::IConnection* connection, LiteNetLib::Utils::NetDataReader* reader, LiteNetLib::DeliveryMethod deliveryMethod) {

}
*/
