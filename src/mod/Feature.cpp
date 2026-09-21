#include "ll/api/base/StdInt.h"
#include "mc/world/level/GameType.h"
#include "mod/MyMod.h"
#include "pland/Global.h"
#include "pland/enums/LandRole.h"

#include <ll/api/event/EventBus.h>
#include <ll/api/event/Listener.h>
#include <ll/api/event/ListenerBase.h>
#include <ll/api/event/player/PlayerDisconnectEvent.h>
#include <ll/api/event/player/PlayerJoinEvent.h>

#include <pland/PLand.h>
#include <pland/events/player/PlayerMoveEvent.h>
#include <pland/land/Land.h>
#include <pland/land/repo/LandRegistry.h>

namespace pland_fly::event {
namespace {
ll::event::ListenerPtr EnterLandListener;
ll::event::ListenerPtr LeaveLandListener;
} // namespace
using namespace land;
inline bool PreCheckLandExistsAndPermission(LandID landId, mce::UUID const& uuid = mce::UUID::EMPTY()) {
    auto& landRegistry = land::PLand::getInstance().getLandRegistry();
    auto  land         = landRegistry.getLand(landId);
    if (
        !land ||                                          // 无领地
        (landRegistry.isOperator(uuid)) ||                // 管理员
        (land->getEffectiveRole(uuid) != LandRole::Actor) // 主人/成员
    ) {
        return true;
    }
    return false;
}

void listen() {
    auto& eventBus = ll::event::EventBus::getInstance();
    EnterLandListener =
        eventBus.emplaceListener<land::event::PlayerEnterLandEvent>([](land::event::PlayerEnterLandEvent const& ev) {
            auto& player = ev.self();
            if (player.getPlayerGameType() != ::GameType::Survival) return;
            auto landId = ev.landId();
            if (PreCheckLandExistsAndPermission(landId, player.getUuid())) {
                player.setAbility(::AbilitiesIndex::MayFly, true);
            }
        });
    LeaveLandListener =
        eventBus.emplaceListener<land::event::PlayerLeaveLandEvent>([](land::event::PlayerLeaveLandEvent const& ev) {
            auto& player = ev.self();
            if (player.getPlayerGameType() != ::GameType::Survival) return;
            player.setAbility(::AbilitiesIndex::MayFly, false);
        });
}

void removeListener() {
    auto& eventBus = ll::event::EventBus::getInstance();
    eventBus.removeListener(EnterLandListener);
    eventBus.removeListener(LeaveLandListener);
}

} // namespace pland_fly::event