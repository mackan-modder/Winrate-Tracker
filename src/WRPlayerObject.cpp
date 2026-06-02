#include <Geode/modify/PlayerObject.hpp>
#include "WRPlayLayer.cpp"

class $modify(WRPlayerObject, PlayerObject){

    void playerDestroyed(bool noEffects) {
        if (!PlayLayer::get()) return;

        geode::log::info("playerDestroyed()");

        auto pl = static_cast<WRPlayLayer*>(PlayLayer::get()); // Special thanks to Jump Markers by TechStudent10

        int startPercentageWithKindness = (pl->m_fields->m_startingPercentage==0.0) ? 0 : static_cast<int>(pl->m_fields->m_startingPercentage+1.5);
        pl->updateWinrate(startPercentageWithKindness,pl->getCurrentPercentInt());
        pl->updateStaticTextLabels();

        PlayerObject::playerDestroyed(noEffects);
    }
};