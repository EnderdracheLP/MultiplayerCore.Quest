#include "hooking.hpp"
#include "BeatSaber/AvatarCore/LimitAvatarPoseRestriction.hpp"
#include "UnityEngine/Vector3.hpp"

// allow players to go anywhere
MAKE_AUTO_HOOK_ORIG_MATCH(AvatarPoseRestrictions_HandleAvatarPoseControllerPositionsWillBeSet, &::BeatSaber::AvatarCore::LimitAvatarPoseRestriction::RestrictPose, void, ::BeatSaber::AvatarCore::LimitAvatarPoseRestriction* self, UnityEngine::Quaternion headRotation, UnityEngine::Vector3 headPosition, UnityEngine::Vector3 leftHandPosition, UnityEngine::Vector3 rightHandPosition, ByRef<UnityEngine::Vector3> newHeadPosition, ByRef<UnityEngine::Vector3> newLeftHandPosition, ByRef<UnityEngine::Vector3> newRightHandPosition) {
    newHeadPosition.heldRef = headPosition;
    newLeftHandPosition.heldRef = self->LimitHandPositionRelativeToHead(leftHandPosition, headPosition);
    newRightHandPosition.heldRef = self->LimitHandPositionRelativeToHead(rightHandPosition, headPosition);
}
