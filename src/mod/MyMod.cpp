#include "mod/MyMod.h"

#include "ll/api/mod/RegisterHelper.h"

namespace pland_fly {

MyMod& MyMod::getInstance() {
    static MyMod instance;
    return instance;
}

bool MyMod::load() {
    getSelf().getLogger().debug("Loading...");
    // Code for loading the mod goes here.
    return true;
}

bool MyMod::enable() {
    getSelf().getLogger().debug("Enabling...");
    // Code for enabling the mod goes here.
    event::listen();
    return true;
}

bool MyMod::disable() {
    getSelf().getLogger().debug("Disabling...");
    // Code for disabling the mod goes here.
    event::removeListener();
    return true;
}

} // namespace pland_fly

LL_REGISTER_MOD(pland_fly::MyMod, pland_fly::MyMod::getInstance());
