#include <Geode/modify/LevelInfoLayer.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>

using namespace geode::prelude;


class $modify(WRLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        std::string m_idCurrent = "1";
        std::string m_nameCurrent = "Stereo Madness";
        std::string m_idPrevious = "1";
        std::string m_namePrevious = "Stereo Madness";
        bool m_isLinked;
        CCMenuItemSpriteExtra* m_buttonLink = nullptr;
        // CCMenuItemSpriteExtra* btn2 = nullptr;

        ~Fields() {
            if (m_idCurrent!=m_idPrevious && m_idCurrent!=Mod::get()->getSavedValue<std::string>("previous-level-id-backup","1")) {
                Mod::get()->setSavedValue("previous-level-id-backup", m_idPrevious);
                Mod::get()->setSavedValue("previous-level-name-backup", m_namePrevious);
                
                Mod::get()->setSavedValue("previous-level-id", m_idCurrent);
                Mod::get()->setSavedValue("previous-level-name", m_nameCurrent);
            } else if (m_idCurrent==Mod::get()->getSavedValue<std::string>("previous-level-id-backup","1")) {
                std::string backupId = Mod::get()->getSavedValue<std::string>("previous-level-id-backup","1");
                std::string backupName = Mod::get()->getSavedValue<std::string>("previous-level-name-backup","1");

                Mod::get()->setSavedValue("previous-level-id-backup", m_idPrevious);
                Mod::get()->setSavedValue("previous-level-name-backup", m_namePrevious);

                Mod::get()->setSavedValue("previous-level-id", backupId);
                Mod::get()->setSavedValue("previous-level-name", backupName);
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

        std::set<std::string> currentLinkedList = Mod::get()->getSavedValue<std::set<std::string>>(m_fields->m_idCurrent + "-linked");

        m_fields->m_isLinked = false;
        for (std::string id : currentLinkedList) {
			if (id==m_fields->m_idPrevious) m_fields->m_isLinked = true;
		}

        // auto spr = ButtonSprite::create("Link");
        auto sprite = CircleButtonSprite::createWithSpriteFrameName("percentage.png"_spr);

        m_fields->m_buttonLink = CCMenuItemSpriteExtra::create(
            sprite,
            nullptr,
            this,
            menu_selector(WRLevelInfoLayer::onLinkButton)
        );
        m_fields->m_buttonLink->setID("wr-link-button-level");
        m_fields->m_buttonLink->setZOrder(1);
        m_fields->m_buttonLink->setScale(0.8);
        m_fields->m_buttonLink->setVisible(true);

        auto folderMenu = getChildByID("left-side-menu");
        folderMenu->addChild(m_fields->m_buttonLink);
        folderMenu->updateLayout();
        
        return true;
    }

    void onLinkButton (CCObject*) {
        if (m_fields->m_isLinked) {
            unlinkPopup();
        } else {
            linkPopup();
        }
    }

    void unlinkPopup() {
        auto alert = geode::createQuickPopup(
			"Un-Link Previous Level?",            // title
			"Do you want to un-link \"" 
            + m_fields->m_namePrevious + "\" from " + m_fields->m_nameCurrent + " all other levels?",   // content
			"Cancel", "Un-Link",      // buttons
			[](auto, bool btn2) {
				if (btn2) {
                    auto CCSC = static_cast<CCScene*>(CCScene::get());

                    auto LIL = static_cast<WRLevelInfoLayer*>(CCSC->getChildByID("EditLevLILayer"));

                    LIL->unlinkLevel(LIL->m_fields->m_idPrevious);
				}
			}
		);
    }

    void linkPopup() {
        auto alert = geode::createQuickPopup(
			"Link to previous level?",            // title
			"Do you want to link the winrate of the levels \"" 
            + m_fields->m_nameCurrent + "\" and \"" 
            + m_fields->m_namePrevious +  "\"?\n",   // content
			"Cancel", "Link",      // buttons
			[](auto, bool btn2) {
				if (btn2) {
                    auto CCSC = static_cast<CCScene*>(CCScene::get());

                    auto LIL = static_cast<WRLevelInfoLayer*>(CCSC->getChildByID("EditLevLILayer"));

                    LIL->linkPopup2();
				}
			}
		);
    }

    void linkPopup2() {
        const char* name1;
        const char* name2;

        if (m_fields->m_nameCurrent==m_fields->m_namePrevious) {
            name1 = m_fields->m_idCurrent.c_str();
            name2 = m_fields->m_idPrevious.c_str();
        } else {
            name1 = m_fields->m_nameCurrent.c_str();
            name2 = m_fields->m_namePrevious.c_str();
        }

        auto alert = geode::createQuickPopup(
			"Choose Levels Winrate",            // title
			"Choose which levels winrate you want to keep for both. \n\"" + m_fields->m_nameCurrent + "\" (" + m_fields->m_idCurrent + ")\n\"" + m_fields->m_namePrevious + "\" (" + m_fields->m_idPrevious + ")\n(Escape to exit)",   // content
			name1, name2,      // buttons
			[](auto, bool btn2) {
                auto CCSC = static_cast<CCScene*>(CCScene::get());

                auto LIL = static_cast<WRLevelInfoLayer*>(CCSC->getChildByID("EditLevLILayer"));
				if (btn2) {
                    LIL->linkLevel(LIL->m_fields->m_idPrevious,LIL->m_fields->m_idCurrent);// something is strange.
				} else {
                    LIL->linkLevel(LIL->m_fields->m_idCurrent,LIL->m_fields->m_idPrevious);// something is strange.
                }
                LIL->m_fields->m_isLinked = true;
			}
		);
    }

    void unlinkLevel(std::string level) {
        auto oldLinked = Mod::get()->getSavedValue<std::set<std::string>>(level + "-linked");
        std::set<std::string> empty;
        empty.insert(level);
        oldLinked.erase(level);
		Mod::get()->setSavedValue(level + "-linked", empty);
        for (std::string id : oldLinked) {
			Mod::get()->setSavedValue(id + "-linked", oldLinked);
		}

        m_fields->m_isLinked = false;
	}

    void linkLevel(std::string levelKeep, std::string levelDicard) {
		auto dataLevelKeep = Mod::get()->getSavedValue<std::set<std::string>>(levelKeep + "-linked");
        dataLevelKeep.insert(levelKeep);

		auto dataLevelDicard = Mod::get()->getSavedValue<std::set<std::string>>(levelDicard + "-linked");
        dataLevelDicard.insert(levelDicard);

		for (std::string id : dataLevelDicard) {
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

        for (std::string id : dataLevelKeep) {
            Mod::get()->setSavedValue(id + "-winrate", tempKeepWinrate);
            Mod::get()->setSavedValue(id + "-datacount", tempKeepDataCount);
            Mod::get()->setSavedValue(id + "-timelength", tempKeepTimeLength);
            Mod::get()->setSavedValue(id + "-linked", dataLevelKeep);
		}
	}
};