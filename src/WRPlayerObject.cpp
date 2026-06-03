#include <Geode/modify/PlayerObject.hpp>
#include "WRPlayLayer.cpp"

class $modify(WRPlayerObject, PlayerObject){

    void playerDestroyed(bool noEffects) {
        if (!PlayLayer::get()) return;

        geode::log::info("playerDestroyed()");

        auto pl = static_cast<WRPlayLayer*>(PlayLayer::get()); // Special thanks to Jump Markers by TechStudent10

        int startPercentage = (pl->m_fields->m_startingPercentage==0.0) ? 0 : pl->m_fields->m_endOfSafeZone;
        pl->updateWinrate(startPercentage,pl->getCurrentPercentInt());
        PlayerObject::playerDestroyed(noEffects);
    }
};