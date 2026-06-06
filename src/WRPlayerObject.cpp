#include <Geode/modify/PlayerObject.hpp>
#include "WRPlayLayer.cpp"

class $modify(WRPlayerObject, PlayerObject){

    void playerDestroyed(bool noEffects) {
        if (!PlayLayer::get()) return;

        geode::log::info("playerDestroyed()");

        auto pl = static_cast<WRPlayLayer*>(PlayLayer::get()); // Special thanks to TechStudent10 where I copied this command from

        int startPercentage = (pl->m_fields->m_startingPercentage==0.0) ? 0 : pl->m_fields->m_endOfSafeZone;
        log::info("destroyed {} {}",pl->m_fields->m_startingPercentage ,pl->m_fields->m_endOfSafeZone);
        pl->updateWinrate(startPercentage,pl->getCurrentPercentInt());
        pl->updateChange();
        PlayerObject::playerDestroyed(noEffects);

        // It worksssssssssssssss! Wait, lol yeah that ruins it but now they are linked.
        // pl->linkLevel("1","2");
        
    }
};