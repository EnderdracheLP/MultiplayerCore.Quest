#include "hooking.hpp"
#include "GlobalNamespace/AvatarPoseRestrictions.hpp"
#include "UnityEngine/Quaternion.hpp"

// allow players to go anywhere
MAKE_AUTO_HOOK_ORIG_MATCH(AvatarPoseRestrictions_HandleAvatarPoseControllerPositionsWillBeSet, &::GlobalNamespace::AvatarPoseRestrictions::HandleAvatarPoseControllerPositionsWillBeSet, void, ::GlobalNamespace::AvatarPoseRestrictions* self, UnityEngine::Quaternion headRotation, UnityEngine::Vector3 headPosition, UnityEngine::Vector3 leftHandPosition, UnityEngine::Vector3 rightHandPosition, ByRef<UnityEngine::Vector3> newHeadPosition, ByRef<UnityEngine::Vector3> newLeftHandPosition, ByRef<UnityEngine::Vector3> newRightHandPosition) {
    newHeadPosition.heldRef = headPosition;
    newLeftHandPosition.heldRef = self->LimitHandPositionRelativeToHead(leftHandPosition, headPosition);
    newRightHandPosition.heldRef = self->LimitHandPositionRelativeToHead(rightHandPosition, headPosition);
}