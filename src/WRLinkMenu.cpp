#include "WRLinkMenu.hpp"

using namespace geode::prelude;





bool WRLinkMenu::init() {

    if (!Popup::init(250,130)) {
        return false;
    }

    this->setTitle("Linking Menu");
    this->setID("WR-Link-Popup"_spr);

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


    auto spriteLink = ButtonSprite::create("Link With Previous Level");
    m_buttonLink = CCMenuItemSpriteExtra::create(
        spriteLink,
        nullptr,
        this,
        menu_selector(WRLinkMenu::onButtonLink)
    );
    m_buttonLink->setID("wr-link-button-previous");
    m_buttonLink->setZOrder(1);
    m_buttonLink->setScale(2);
    m_buttonLink->setVisible(true);
    myMenuContent->addChildAtPosition(m_buttonLink,Anchor::Top);

    auto spriteUnLink = ButtonSprite::create("Unlink Current Level");
    m_buttonUnLink = CCMenuItemSpriteExtra::create(
        spriteUnLink,
        nullptr,
        this,
        menu_selector(WRLinkMenu::onUnLinkCurrent)
    );
    m_buttonUnLink->setID("wr-link-button-unlink");
    m_buttonUnLink->setZOrder(1);
    m_buttonUnLink->setScale(2);
    m_buttonUnLink->setVisible(true);
    myMenuContent->addChildAtPosition(m_buttonUnLink,Anchor::Top);

    auto spriteLinkedList = ButtonSprite::create("Linked Levels");
    m_buttonLinkedList = CCMenuItemSpriteExtra::create(
        spriteLinkedList,
        nullptr,
        this,
        menu_selector(WRLinkMenu::onLinkedList)
    );
    m_buttonLinkedList->setID("wr-link-button-list");
    m_buttonLinkedList->setZOrder(1);
    m_buttonLinkedList->setScale(2);
    m_buttonLinkedList->setVisible(true);
    myMenuContent->addChildAtPosition(m_buttonLinkedList,Anchor::Top);

    // auto label1 = CCLabelBMFont::create("Winrate Tracker Menu", "bigFont.fnt");
    // label1->setScale(0.6);
    // myMenuContent->addChildAtPosition(label1, Anchor::Top);
    // auto label2 = CCLabelBMFont::create("Winrate Tracker Menu", "bigFont.fnt");
    // label2->setScale(0.6);
    // myMenuContent->addChildAtPosition(label2, Anchor::Top);
    // auto label3 = CCLabelBMFont::create("Winrate Tracker Menu", "bigFont.fnt");
    // label3->setScale(0.6);
    // myMenuContent->addChildAtPosition(label3, Anchor::Top);

    myMenuContent->updateLayout();

    CCPoint offset = {0,-75};
    CCPoint offsetBackground = {0,-10};
    m_mainLayer->addChildAtPosition(background,Anchor::Center,offsetBackground);
    m_mainLayer->addChildAtPosition(myMenuContent, Anchor::Center, offset);
    

    m_mainLayer->updateLayout();



    // Finding if the the current level is already linked with the previous
    gd::set<gd::string> currentLinkedList = Mod::get()->getSavedValue<gd::set<gd::string>>(m_idCurrent + "-linked");

    m_isLinked = false;
    for (gd::string id : currentLinkedList) {
        if (id==m_idPrevious) m_isLinked = true;
    }

    return true;
}


WRLinkMenu* WRLinkMenu::create(gd::string idCurrent, gd::string nameCurrent, gd::string idPrevious, gd::string namePrevious) {
    auto ret = new WRLinkMenu();
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

void WRLinkMenu::onButtonLink(CCObject*) {
    if (m_isLinked) {
        unlinkPreviousPopup();
    } else {
        linkPopup();
    }
    return;
}

void WRLinkMenu::unlinkPreviousPopup() {
    auto alert = geode::createQuickPopup(
        "Un-Link Previous Level?",            // title
        "Do you want to un-link \"" 
        + this->m_namePrevious + "\" from " + this->m_nameCurrent + " and all other levels?",   // content
        "Cancel", "Un-Link",      // buttons
        [this](auto, bool btn2) {
            if (btn2) {
                this->unlinkLevel(this->m_idPrevious);
            }
        }
    );
}

void WRLinkMenu::onUnLinkCurrent(CCObject*) {
    auto alert = geode::createQuickPopup(
        "Un-Link Previous Level?",            // title
        "Do you want to un-link \"" 
        + this->m_nameCurrent + "\" from all other levels?",   // content
        "Cancel", "Un-Link",      // buttons
        [this](auto, bool btn2) {
            if (btn2) {
                this->unlinkLevel(this->m_idCurrent);
            }
        }
    );
}

void WRLinkMenu::onLinkedList(CCObject*){

    gd::set<gd::string> linkedList = Mod::get()->getSavedValue<gd::set<gd::string>>(m_idCurrent+"-linked");
    linkedList.erase(m_idCurrent);

    gd::set<gd::string>::iterator id;

    

    gd::string levelsString = m_nameCurrent + " is linked with:\n";

    int size = linkedList.size();
    int i = 0;
    for (id = linkedList.begin(); id != linkedList.end(); id++) {
        gd::string localString = Mod::get()->getSavedValue<gd::string>(*id + "-levelname", "");
        if (localString!="") {
            levelsString += localString;
        } else {
            levelsString += *id;
        }
        if (id==linkedList.end()) {
            levelsString += ".";
        } else if (i==size-2) {
            levelsString += " and ";
        } else {
            levelsString += ", ";
        }
        i++;
    }



    FLAlertLayer::create(
    "List of linked levels",    // title
    levelsString,  // content
    "OK"        // button
    )->show();

    return;
}

void WRLinkMenu::onResetWinrate(CCObject*) {
    auto alert = geode::createQuickPopup(
        "Reset Winrate?",            // title
        "Are you sure you want to reset the winrate of \"" 
        + this->m_nameCurrent + "?\n",   // content
        "Cancel", "Reset",      // buttons
        [this](auto, bool btn2) {
            if (btn2) {
                gd::string id = this->m_idCurrent;

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

void WRLinkMenu::onLink(CCObject*) {
    

    return;
}

void WRLinkMenu::linkPopup() {
        auto alert = geode::createQuickPopup(
			"Link to previous level?",            // title
			"Do you want to link the winrate of the levels \"" 
            + this->m_nameCurrent + "\" and \"" 
            + this->m_namePrevious +  "\"?\n",   // content
			"Cancel", "Link",      // buttons
			[this](auto, bool btn2) {
				if (btn2) {
                    this->linkPopup2();
				}
			}
		);
    }

void WRLinkMenu::linkPopup2() {
        const char* name1;
        const char* name2;

        // This is to distinguish levels with the same name 
        if (this->m_nameCurrent==this->m_namePrevious) {
            name1 = this->m_idCurrent.c_str();
            name2 = this->m_idPrevious.c_str();
        } else {
            name1 = this->m_nameCurrent.c_str();
            name2 = this->m_namePrevious.c_str();
        }

        auto alert = geode::createQuickPopup(
			"Choose Levels Winrate",            // title
			"Choose which levels winrate you want to keep for both. \n\"" + this->m_nameCurrent + "\" (" + this->m_idCurrent + ")\n\"" + this->m_namePrevious + "\" (" + this->m_idPrevious + ")\n(Escape to exit)",   // content
			name1, name2,      // buttons
			[this](auto, bool btn2) {
				if (btn2) {
                    this->linkLevel(this->m_idPrevious,this->m_idCurrent);
				} else {
                    this->linkLevel(this->m_idCurrent,this->m_idPrevious);
                }
                this->m_isLinked = true;
			}
		);
    }

void WRLinkMenu::unlinkLevel(gd::string level) {
    auto oldLinked = Mod::get()->getSavedValue<gd::set<gd::string>>(level + "-linked");
    gd::set<gd::string> empty;
    empty.insert(level);
    oldLinked.erase(level);
    Mod::get()->setSavedValue(level + "-linked", empty);
    for (gd::string id : oldLinked) {
        Mod::get()->setSavedValue(id + "-linked", oldLinked);
    }

    this->m_isLinked = false;
}



void WRLinkMenu::linkLevel(gd::string levelKeep, gd::string levelDicard) {
    auto dataLevelKeep = Mod::get()->getSavedValue<gd::set<gd::string>>(levelKeep + "-linked");
    dataLevelKeep.insert(levelKeep);

    auto dataLevelDicard = Mod::get()->getSavedValue<gd::set<gd::string>>(levelDicard + "-linked");
    dataLevelDicard.insert(levelDicard);

    for (gd::string id : dataLevelDicard) {
        dataLevelKeep.insert(id);
    }

    // Making default values
    std::array<float,100> tempKeepWinrate;
    std::array<int,100> tempKeepDataCount;
    std::array<float,100> tempKeepTimeLength;

    for(int i=0;i<100;i++){	
        tempKeepWinrate[i] = 1.0;
        tempKeepDataCount[i] = 0;
        tempKeepTimeLength[i] = -1;
    }

    tempKeepWinrate = Mod::get()->getSavedValue<std::array<float,100>>(levelKeep + "-winrate", tempKeepWinrate);
    tempKeepDataCount = Mod::get()->getSavedValue<std::array<int,100>>(levelKeep + "-datacount", tempKeepDataCount);
    tempKeepTimeLength = Mod::get()->getSavedValue<std::array<float,100>>(levelKeep + "-timelength", tempKeepTimeLength);

    for (gd::string id : dataLevelKeep) {
        Mod::get()->setSavedValue(id + "-winrate", tempKeepWinrate);
        Mod::get()->setSavedValue(id + "-datacount", tempKeepDataCount);
        Mod::get()->setSavedValue(id + "-timelength", tempKeepTimeLength);
        Mod::get()->setSavedValue(id + "-linked", dataLevelKeep);
    }
}