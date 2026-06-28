
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include "WRPlayLayer.cpp"
#include "WRStatsMenu.hpp"


using namespace geode::prelude;


// class $modify(PTPauseLayer, PauseLayer) {
// 	struct Fields {
//         CCMenuItemSpriteExtra* m_buttonStats = nullptr;
//     };
// };

class $modify(WRPauseLayer,PauseLayer) {
    struct Fields {
        CCMenuItemSpriteExtra* m_buttonStats = nullptr;
        std::string m_levelID = "-1";
        std::string m_levelName = "-1";
    };

    void customSetup() {
        PauseLayer::customSetup();

        auto spr 
        = CircleButtonSprite::createWithSprite("percentage.png"_spr);

        spr->setScale(0.8);

        m_fields->m_buttonStats = CCMenuItemSpriteExtra::create(
            spr,
            nullptr,
            this,
            menu_selector(WRPauseLayer::onStats)
        );
        m_fields->m_buttonStats->setID("wr-pause-stats");
        m_fields->m_buttonStats->setZOrder(1);
        m_fields->m_buttonStats->setVisible(true);

        auto leftMenu = this->getChildByID("left-button-menu");
        leftMenu->addChild(m_fields->m_buttonStats);
        leftMenu->updateLayout();

        if (PlayLayer::get()) {
            auto pl = static_cast<WRPlayLayer*>(PlayLayer::get());

            m_fields->m_levelID = pl->m_fields->m_levelId;
            m_fields->m_levelName = pl->m_fields->m_levelName;

            Mod::get()->setSavedValue(pl->m_fields->m_levelId 
            + "-winrate", pl->m_fields->m_percentageWinrate);

            Mod::get()->setSavedValue(pl->m_fields->m_levelId 
            + "-datacount", pl->m_fields->m_percentageDataCount);

            Mod::get()->setSavedValue(pl->m_fields->m_levelId 
            + "-timelength", pl->m_fields->m_percentageTimeLength);

            Mod::get()->setSavedValue(pl->m_fields->m_levelId 
            + "-linked", pl->m_fields->m_linkedLevels);

		    Mod::get()->setSavedValue(pl->m_fields->m_levelId 
            + "-attempts", pl->m_attempts+pl->m_fields->m_attemptsStart);
        }
    }

    void onStats(CCObject*) {
        WRStatsMenu::create(this->m_fields->m_levelID
        ,this->m_fields->m_levelName)->show();

        return;
    }
};