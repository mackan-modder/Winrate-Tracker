#include "WRMenu.hpp"
#include "WRLinkMenu.hpp"
#include "constants.cpp"
#include "WRStatsMenu.hpp"

using namespace geode::prelude;

bool WRMenu::init() {

    if (!Popup::init(250,130)) {
        return false;
    }

    this->setTitle("Winrate Tracker");
    this->setID("WR-Popup"_spr);

    // auto label = CCLabelBMFont::create("Winrate Tracker Menu", "bigFont.fnt");
    // label->setScale(0.6); 
    // m_mainLayer->addChildAtPosition(label, Anchor::Top, CCPoint(0.f,-20.f));
    auto myMenuContent = CCMenu::create();
    myMenuContent->setLayout(ColumnLayout::create()->setAxisReverse(true)->setAxisAlignment(AxisAlignment::End));
    myMenuContent->setScale(0.6);

    auto background = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
    background->setColor({ 0, 0, 0 });
    background->setOpacity(100);
    background->setContentSize({ 225.f, 85.f });
    background->setID("WR-background");

    auto spriteLink = ButtonSprite::create("Link");
    m_buttonLink = CCMenuItemSpriteExtra::create(
        spriteLink,
        nullptr,
        this,
        menu_selector(WRMenu::onLink)
    );
    m_buttonLink->setID("wr-button-Link");
    m_buttonLink->setZOrder(1);
    m_buttonLink->setScale(2);
    m_buttonLink->setVisible(true);
    myMenuContent->addChildAtPosition(m_buttonLink,Anchor::Top);


    auto spriteStats = ButtonSprite::create("Winrate Stats");
    m_buttonStats = CCMenuItemSpriteExtra::create(
        spriteStats,
        nullptr,
        this,
        menu_selector(WRMenu::onStats)
    );
    m_buttonStats->setID("wr-button-stats");
    m_buttonStats->setZOrder(1);
    m_buttonStats->setScale(2);
    m_buttonStats->setVisible(true);
    myMenuContent->addChildAtPosition(m_buttonStats,Anchor::Top);
    auto spriteReset = ButtonSprite::create("Reset Winrate of level");
    m_buttonReset = CCMenuItemSpriteExtra::create(
        spriteReset,
        nullptr,
        this,
        menu_selector(WRMenu::onResetWinrate)
    );
    m_buttonReset->setID("wr-button-reset");
    m_buttonReset->setZOrder(1);
    m_buttonReset->setScale(2);
    m_buttonReset->setVisible(true);
    myMenuContent->addChildAtPosition(m_buttonReset,Anchor::Top);

    myMenuContent->updateLayout();

    CCPoint offset = {0,-75};
    CCPoint offsetBackground = {0,-10};
    m_mainLayer->addChildAtPosition(background,Anchor::Center,offsetBackground);
    m_mainLayer->addChildAtPosition(myMenuContent, Anchor::Center, offset);
    
    m_mainLayer->updateLayout();

    return true;
}


WRMenu* WRMenu::create(std::string idCurrent, std::string nameCurrent, std::string idPrevious, std::string namePrevious) {
    auto ret = new WRMenu();
    ret->m_idCurrent = idCurrent;
    ret->m_nameCurrent = nameCurrent;
    ret->m_idPrevious = idPrevious;
    ret->m_namePrevious = namePrevious;
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void WRMenu::onResetWinrate(CCObject*) {
    auto alert = geode::createQuickPopup(
        "Reset Winrate?",            // title
        "Are you sure you want to reset the winrate of \"" 
        + this->m_nameCurrent + "?\n",   // content
        "Cancel", "Reset",      // buttons
        [this](auto, bool btn2) {
            if (btn2) {
                std::string id = this->m_idCurrent;

                std::array<float,100> defaultWinrate;
                std::array<int,100> defaultDataCount;
                std::array<float,100> defaultTimeLength;

                for(int i=0;i<100;i++){	
                    defaultWinrate[i] = 1.0;
                    defaultDataCount[i] = 0;
                    defaultTimeLength[i] = -1;
                }

                Mod::get()->setSavedValue(id + "-winrate", defaultWinrate);
				Mod::get()->setSavedValue(id+ "-datacount", defaultDataCount);
				Mod::get()->setSavedValue(id + "-timelength", defaultTimeLength);
            }
        }
    );

    return;
}

void WRMenu::onLink(CCObject*) {
    WRLinkMenu::create(this->m_idCurrent, this->m_nameCurrent, this->m_idPrevious, this->m_namePrevious)->show();
    this->removeMeAndCleanup();

    return;
}

void WRMenu::onStats(CCObject*) {
    // Yeah I decided to implement this today anyway(a basic version of it anyway).

    WRStatsMenu::create(this->m_idCurrent,this->m_nameCurrent)->show();
    this->removeMeAndCleanup();

    return;
}