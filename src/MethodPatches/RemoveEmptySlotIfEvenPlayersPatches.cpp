#include "hooking.hpp"
#include "logging.hpp"

#include "GlobalNamespace/MultiplayerPlayersManager.hpp"
#include "GlobalNamespace/MultiplayerLobbyCenterStageManager.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarPlaceManager.hpp"
#include "GlobalNamespace/MultiplayerLobbyAvatarManager.hpp"
#include "beatsaber-hook/shared/utils/capstone-utils.hpp"
#include <sys/mman.h>

#define __attribute                __attribute__
#define aligned(x)                 __aligned__(x)
#define __intval(p)                reinterpret_cast<intptr_t>(p)
#define __uintval(p)               reinterpret_cast<uintptr_t>(p)
#define __ptr(p)                   reinterpret_cast<void *>(p)
#define __page_size                4096
#define __page_align(n)            __align_up(static_cast<uintptr_t>(n), __page_size)
#define __ptr_align(x)             __ptr(__align_down(reinterpret_cast<uintptr_t>(x), __page_size))
#define __align_up(x, n)           (((x) + ((n) - 1)) & ~((n) - 1))
#define __align_down(x, n)         ((x) & -(n))
#define __countof(x)               static_cast<intptr_t>(sizeof(x) / sizeof((x)[0])) // must be signed
#define __atomic_increase(p)       __sync_add_and_fetch(p, 1)
#define __sync_cmpswap(p, v, n)    __sync_bool_compare_and_swap(p, v, n)
#define __predict_true(exp)        __builtin_expect((exp) != 0, 1)

#define __make_rwx(p, n)           ::mprotect(__ptr_align(p), \
                                              __page_align(__uintval(p) + n) != __page_align(__uintval(p)) ? __page_align(n) + __page_size : __page_align(n), \
                                              PROT_READ | PROT_WRITE | PROT_EXEC)

template<int ins>
std::optional<uint32_t*> findIns(cs_insn* insn) {
    return (insn->id == ins) ? std::optional<uint32_t*>(reinterpret_cast<uint32_t*>(insn->address)) : std::nullopt;
}

bool set_ins(uint32_t* pos, uint32_t instruction) {
    static auto il2cpp_base = baseAddr("libil2cpp.so");
    if (__make_rwx(pos, sizeof(uint32_t)) == 0) {
        *pos = instruction;
        __builtin___clear_cache(reinterpret_cast<char *>(pos), reinterpret_cast<char *>(pos) + 1u);
        return true;
    }

    ERROR("Was unable to mark instruction @ {:x} as read-write, unable to edit instruction!", ((uintptr_t)pos) - il2cpp_base);
    return false;
}

void Patch_MultiplayerPlayersManager_SpawnPlayers(Logger& logger) {
    static auto il2cpp_base = baseAddr("libil2cpp.so");

    // patch MultiplayerPlayersManager_SpawnPlayers check to always ignore an extra player to get uneven count
    auto minfo = il2cpp_utils::il2cpp_type_check::MetadataGetter<&::GlobalNamespace::MultiplayerPlayersManager::SpawnPlayers>::get();
    auto mptr = (uint32_t*)minfo->methodPointer;

    DEBUG("Method pointer : {} (offset {:x})", fmt::ptr(mptr), ((uintptr_t)mptr) - il2cpp_base);
    auto cset = cs::findNth<2, &findIns<ARM64_INS_CSET>, &cs::insnMatch<>, 1>(mptr);
    if (cset) {
        auto ins = *cset;
        DEBUG("Found 2nd cset @ {} (offset {:x})", fmt::ptr(ins), ((uintptr_t)ins) - il2cpp_base);
        DEBUG("Instruction before edit: {:x}", *ins);
        if (*ins == 0x1a9f07e9u) { // check for 'cset w9, ne'
            if (!set_ins(ins, 0x52800009u)) { // mov w9, 0x0
                ERROR("Failed to set instruction in MultiplayerPlayersManager_SpawnPlayers");
            }
        } else {
            ERROR("Instruction match for MultiplayerPlayersManager_SpawnPlayers failed, please report this to the mpcore Developers!");
        }
    }
}
AUTO_INSTALL_PATCH(MultiplayerPlayersManager_SpawnPlayers);

void Patch_MultiplayerLobbyCenterStageManager_RecalculateCenterPosition(Logger& logger) {
    static auto il2cpp_base = baseAddr("libil2cpp.so");

    // patch MultiplayerLobbyCenterStageManager_RecalculateCenterPosition check to always ignore an extra player to get uneven count
    auto minfo = il2cpp_utils::il2cpp_type_check::MetadataGetter<&::GlobalNamespace::MultiplayerLobbyCenterStageManager::RecalculateCenterPosition>::get();
    auto mptr = (uint32_t*)minfo->methodPointer;

    DEBUG("Method pointer : {} (offset {:x})", fmt::ptr(mptr), ((uintptr_t)mptr) - il2cpp_base);
    auto andIns = cs::findNth<1, &findIns<ARM64_INS_AND>, &cs::insnMatch<>, 1>(mptr);
    if (andIns) {
        auto ins = *andIns;
        DEBUG("Found 1st and @ {} (offset {:x})", fmt::ptr(ins), ((uintptr_t)ins) - il2cpp_base);
        DEBUG("Instruction before edit: {:x}", *ins);
        if (*ins == 0x12000129u) { // check for 'and w9, w9, 0x1'
            if (!set_ins(ins, 0x52800009u)) { // mov w9, 0x0
                ERROR("Failed to set instruction in MultiplayerLobbyCenterStageManager_RecalculateCenterPosition");
            }
        } else {
            ERROR("Instruction match for MultiplayerLobbyCenterStageManager_RecalculateCenterPosition failed, please report this to the mpcore Developers!");
        }
    }
}
AUTO_INSTALL_PATCH(MultiplayerLobbyCenterStageManager_RecalculateCenterPosition);

void Patch_MultiplayerLobbyAvatarPlaceManager_SpawnAllPlaces(Logger& logger) {
    static auto il2cpp_base = baseAddr("libil2cpp.so");

    // patch MultiplayerLobbyAvatarPlaceManager_SpawnAllPlaces check to always ignore an extra player to get uneven count
    auto minfo = il2cpp_utils::il2cpp_type_check::MetadataGetter<&::GlobalNamespace::MultiplayerLobbyAvatarPlaceManager::SpawnAllPlaces>::get();
    auto mptr = (uint32_t*)minfo->methodPointer;

    DEBUG("Method pointer : {} (offset {:x})", fmt::ptr(mptr), ((uintptr_t)mptr) - il2cpp_base);
    auto andIns = cs::findNth<1, &findIns<ARM64_INS_AND>, &cs::insnMatch<>, 1>(mptr);
    if (andIns) {
        auto ins = *andIns;
        DEBUG("Found 1st and @ {} (offset {:x})", fmt::ptr(ins), ((uintptr_t)ins) - il2cpp_base);
        DEBUG("Instruction before edit: {:x}", *ins);
        if (*ins == 0x12000129u) { // check for 'and w9, w9, 0x1'
            if (!set_ins(ins, 0x52800009u)) { // mov w9, 0x0
                ERROR("Failed to set instruction in MultiplayerLobbyAvatarPlaceManager_SpawnAllPlaces");
            }
        } else {
            ERROR("Instruction match for MultiplayerLobbyAvatarPlaceManager_SpawnAllPlaces failed, please report this to the mpcore Developers!");
        }
    }
}
AUTO_INSTALL_PATCH(MultiplayerLobbyAvatarPlaceManager_SpawnAllPlaces);

void Patch_MultiplayerLobbyAvatarManager_AddPlayer(Logger& logger) {
    static auto il2cpp_base = baseAddr("libil2cpp.so");

    // patch MultiplayerLobbyAvatarManager_AddPlayer check to always ignore an extra player to get uneven count
    auto minfo = il2cpp_utils::il2cpp_type_check::MetadataGetter<&::GlobalNamespace::MultiplayerLobbyAvatarManager::AddPlayer>::get();
    auto mptr = (uint32_t*)minfo->methodPointer;

    DEBUG("Method pointer : {} (offset {:x})", fmt::ptr(mptr), ((uintptr_t)mptr) - il2cpp_base);
    auto andIns = cs::findNth<1, &findIns<ARM64_INS_AND>, &cs::insnMatch<>, 1>(mptr);
    if (andIns) {
        auto ins = *andIns;
        DEBUG("Found 1st and @ {} (offset {:x})", fmt::ptr(ins), ((uintptr_t)ins) - il2cpp_base);
        DEBUG("Instruction before edit: {:x}", *ins);
        if (*ins == 0x12000129u) { // check for 'and w9, w9, 0x1'
            if (set_ins(ins, 0x52800009u)) { // mov w9, 0x0
                ERROR("Failed to set instruction in MultiplayerLobbyAvatarManager_AddPlayer");
            }
        } else {
            ERROR("Instruction match for MultiplayerLobbyAvatarManager_AddPlayer failed, please report this to the mpcore Developers!");
        }
    }
}
AUTO_INSTALL_PATCH(MultiplayerLobbyAvatarManager_AddPlayer);
