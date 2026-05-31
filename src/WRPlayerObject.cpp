#include <Geode/modify/PlayerObject.hpp>
#include "WRPlayLayer.cpp"

class $modify(WRPlayerObject, PlayerObject){

    void playerDestroyed(bool noEffects) {
        if (!PlayLayer::get()) return;

        geode::log::info("playerDestroyed()");

        auto pl = static_cast<WRPlayLayer*>(PlayLayer::get()); // Special thanks to Jump Markers by TechStudent10

        pl->updateWinrate(pl->m_fields->m_startingPercentage,pl->getCurrentPercentInt());

        PlayerObject::playerDestroyed(noEffects);
    }
};