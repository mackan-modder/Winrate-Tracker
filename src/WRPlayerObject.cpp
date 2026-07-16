#include <Geode/modify/PlayerObject.hpp>
#include "WRPlayLayer.cpp"

class $modify(WRPlayerObject, PlayerObject){

    void playerDestroyed(bool noEffects) {
        if (!PlayLayer::get()) return;

        // log::info("playerDestroyed()");

        auto pl = static_cast<WRPlayLayer*>(PlayLayer::get());

        int startPercentage 
        = (pl->m_fields->m_startingPercentage==0.0) ? 
        0 : pl->m_fields->m_endOfSafeZone;

        // log::info("destroyed {} {}",pl->m_fields->m_startingPercentage 
        // ,pl->m_fields->m_endOfSafeZone);
        
        pl->updateWinrate(startPercentage,pl->getCurrentPercentInt(),false);
        pl->updateChange();
        PlayerObject::playerDestroyed(noEffects);
    }
};

