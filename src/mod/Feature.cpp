#include "mc/world/level/GameType.h"
#include "mod/MyMod.h"

#include <ll/api/event/EventBus.h>
#include <ll/api/event/Listener.h>
#include <ll/api/event/ListenerBase.h>
#include <ll/api/event/player/PlayerDisconnectEvent.h>
#include <ll/api/event/player/PlayerJoinEvent.h>

#include <pland/PLand.h>
#include <pland/land/LandEvent.h>
#include <pland/land/LandRegistry.h>

namespace pland_fly::event {
namespace {
ll::event::ListenerPtr EnterLandListener;
ll::event::ListenerPtr LeaveLandListener;
} // namespace
using namespace land;
inline bool PreCheckLandExistsAndPermission(SharedLand const& ptr, UUIDs const& uuid = "") {
    if (!ptr ||                                                       // 无领地
        (PLand::getInstance().getLandRegistry()->isOperator(uuid)) || // 管理员
        (ptr->getPermType(uuid) != LandPermType::Guest)               // 主人/成员
    ) {
        return true;
    }
    return false;
}

void listen() {
    auto& eventBus    = ll::event::EventBus::getInstance();
    EnterLandListener = eventBus.emplaceListener<PlayerEnterLandEvent>([](PlayerEnterLandEvent const& ev) {
        auto& player = ev.getPlayer();
        if (player.getPlayerGameType() != ::GameType::Survival) return;
        auto  landId = ev.getLandID();
        auto* db     = PLand::getInstance().getLandRegistry();
        auto  land   = db->getLand(landId);
        if (PreCheckLandExistsAndPermission(land, player.getUuid().asString())) {
            player.setAbility(::AbilitiesIndex::MayFly, true);
        }
    });
    LeaveLandListener = eventBus.emplaceListener<PlayerLeaveLandEvent>([](PlayerLeaveLandEvent const& ev) {
        auto& player = ev.getPlayer();
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