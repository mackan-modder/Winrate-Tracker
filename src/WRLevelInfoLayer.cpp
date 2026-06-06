#include <Geode/modify/LevelInfoLayer.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>

using namespace geode::prelude;


class $modify(WRLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        std::string m_idCurrent = "1";
        std::string m_nameCurrent = "Stereo Madness";
        std::string m_idPrevious = "1";
        std::string m_namePrevious = "Stereo Madness";
        CCMenuItemSpriteExtra* btn = nullptr;
        CCMenuItemSpriteExtra* btn2 = nullptr;

        ~Fields() {
            Mod::get()->setSavedValue("previous-level-id", m_idCurrent);
            Mod::get()->setSavedValue("previous-level-name", m_nameCurrent);

            if (Mod::get()->getSavedValue<std::string>("previous-level-id-backup","1")!=m_idCurrent) {
                Mod::get()->setSavedValue("previous-level-id-backup", m_idPrevious);
                Mod::get()->setSavedValue("previous-level-name-backup", m_namePrevious);
            }
        }
    };

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        int ourLevelId = level->m_levelID.value();
		if (!ourLevelId) {
			ourLevelId = EditorIDs::getID(level);
		}

        
        m_fields->m_idCurrent = std::to_string(ourLevelId);
        m_fields->m_nameCurrent = level->m_levelName;
        
        m_fields->m_idPrevious = Mod::get()->getSavedValue<std::string>("previous-level-id","1");
        m_fields->m_namePrevious = Mod::get()->getSavedValue<std::string>("previous-level-name","Stereo Madness");

        if (m_fields->m_idPrevious==m_fields->m_idCurrent) {
            m_fields->m_idPrevious = Mod::get()->getSavedValue<std::string>("previous-level-id-backup","1");
            m_fields->m_namePrevious = Mod::get()->getSavedValue<std::string>("previous-level-name-backup","Stereo Madness");
        }


        auto spr = ButtonSprite::create("Link");

        m_fields->btn = CCMenuItemSpriteExtra::create(
            spr,
            nullptr,
            this,
            menu_selector(WRLevelInfoLayer::linkPopup)
        );
        m_fields->btn->setID("wr-link-button-level");
        m_fields->btn->setZOrder(1);
        m_fields->btn->setVisible(true);

        auto otherMenu = getChildByID("left-side-menu");
        otherMenu->addChild(m_fields->btn);
        otherMenu->updateLayout();

        auto spr2 = ButtonSprite::create("Un-Link");

        m_fields->btn2 = CCMenuItemSpriteExtra::create(
            spr2,
            nullptr,
            this,
            menu_selector(WRLevelInfoLayer::unlinkPopup)
        );
        m_fields->btn2->setID("wr-link-button-level");
        m_fields->btn2->setZOrder(1);
        m_fields->btn2->setVisible(true);

        otherMenu->addChild(m_fields->btn2);
        otherMenu->updateLayout();
        
        return true;
    }

    void linkPopup(CCObject*) {

        auto alert = geode::createQuickPopup(
			"Link to previous level?",            // title
			"Do you want to link the winrate of the levels \"" 
            + m_fields->m_nameCurrent + "\" and \"" 
            + m_fields->m_namePrevious +  "\"?",   // content
			"Cancel", "Yes",      // buttons
			[](auto, bool btn2) {
				if (btn2) {
                    auto CCSC = static_cast<CCScene*>(CCScene::get());

                    auto LIL = static_cast<WRLevelInfoLayer*>(CCSC->getChildByID("LevelInfoLayer"));

                    LIL->linkPopup2();

                    // LIL->linkLevel("1","2");
				}
			}
		);
    }

    void unlinkPopup(CCObject*) {

        auto alert = geode::createQuickPopup(
			"Un-Link Level?",            // title
			"Do you want to un-link \"" 
            + m_fields->m_nameCurrent + "\" from all other levels?",   // content
			"Cancel", "Un-Link",      // buttons
			[](auto, bool btn2) {
				if (btn2) {
                    auto CCSC = static_cast<CCScene*>(CCScene::get());

                    auto LIL = static_cast<WRLevelInfoLayer*>(CCSC->getChildByID("LevelInfoLayer"));

                    LIL->unlinkLevel(LIL->m_fields->m_idCurrent);
				}
			}
		);
    }

    void linkPopup2() {
        auto alert = geode::createQuickPopup(
			"Choose Level",            // title
			"Choose which levels winrate you want to keep. \nLevel 1: \"" + m_fields->m_nameCurrent + "\" (" + m_fields->m_idCurrent + ")\nLevel 2: \"" + m_fields->m_namePrevious + "\" (" + m_fields->m_idPrevious + ")",   // content
			"Level 1", "Level 2",      // buttons
			[](auto, bool btn2) {
                auto CCSC = static_cast<CCScene*>(CCScene::get());

                auto LIL = static_cast<WRLevelInfoLayer*>(CCSC->getChildByID("LevelInfoLayer"));
				if (btn2) {
                    LIL->linkLevel(LIL->m_fields->m_idPrevious,LIL->m_fields->m_idCurrent);
				} else {
                    LIL->linkLevel(LIL->m_fields->m_idCurrent,LIL->m_fields->m_idPrevious);
                }
			}
		);
    }

    void unlinkLevel(std::string level) {
        std::set<std::string> empty;
        empty.insert(level);
		Mod::get()->setSavedValue(level + "-linked", empty);
	}

    void linkLevel(std::string levelKeep, std::string levelDicard) {
		auto dataLevelKeep = Mod::get()->getSavedValue<std::set<std::string>>(levelKeep + "-linked");
        dataLevelKeep.insert(levelKeep);

		auto dataLevelDicard = Mod::get()->getSavedValue<std::set<std::string>>(levelDicard + "-linked");
        dataLevelDicard.insert(levelDicard);

		for (std::string id : dataLevelDicard) {
			dataLevelKeep.insert(id);
		}

        for (std::string id : dataLevelKeep) {
			Mod::get()->setSavedValue(id + "-linked", dataLevelKeep);
		}
	}
};