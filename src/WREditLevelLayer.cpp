#include <Geode/modify/EditLevelLayer.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include "WRMenu.hpp"

using namespace geode::prelude;

class $modify(WREditLevelLayer, EditLevelLayer) {
    struct Fields {
        std::string m_idCurrent = "1";
        std::string m_nameCurrent = "Stereo Madness";
        std::string m_idPrevious = "1";
        std::string m_namePrevious = "Stereo Madness";
        CCMenuItemSpriteExtra* m_buttonMenu = nullptr;

        ~Fields() {
            if (m_idCurrent!=m_idPrevious 
            && m_idCurrent!=Mod::get()->getSavedValue<std::string>(
            "previous-level-id-backup","1")) {
                Mod::get()->setSavedValue("previous-level-id-backup"
                , m_idPrevious);
                Mod::get()->setSavedValue("previous-level-name-backup"
                , m_namePrevious);
                
                Mod::get()->setSavedValue("previous-level-id"
                , m_idCurrent);
                Mod::get()->setSavedValue("previous-level-name"
                , m_nameCurrent);
            } else if (m_idCurrent==Mod::get()->getSavedValue<std::string>(
            "previous-level-id-backup","1")) {
                std::string backupId = Mod::get()->getSavedValue<std::string>(
                "previous-level-id-backup","1");

                std::string backupName = Mod::get()->getSavedValue<std::string>(
                "previous-level-name-backup","1");

                Mod::get()->setSavedValue("previous-level-id-backup"
                , m_idPrevious);
                Mod::get()->setSavedValue("previous-level-name-backup"
                , m_namePrevious);

                Mod::get()->setSavedValue("previous-level-id"
                , backupId);
                Mod::get()->setSavedValue("previous-level-name"
                , backupName);
            }           
        }
    };


    bool init(GJGameLevel* level) {
        if (!EditLevelLayer::init(level)) return false;

        int ourLevelId = level->m_levelID.value();
		if (!ourLevelId) {
			ourLevelId = EditorIDs::getID(level);
		}

        m_fields->m_idCurrent = fmt::to_string(ourLevelId);
        m_fields->m_nameCurrent = level->m_levelName;
        
        m_fields->m_idPrevious = Mod::get()->getSavedValue<std::string>(
        "previous-level-id","1");

        m_fields->m_namePrevious = Mod::get()->getSavedValue<std::string>(
        "previous-level-name","Stereo Madness");

        if (m_fields->m_idPrevious==m_fields->m_idCurrent) {
            m_fields->m_idPrevious = Mod::get()->getSavedValue<std::string>(
            "previous-level-id-backup","1");

            m_fields->m_namePrevious = Mod::get()->getSavedValue<std::string>(
            "previous-level-name-backup","Stereo Madness");
        }
        
        auto spr 
        = CircleButtonSprite::createWithSprite("percentage.png"_spr);

        spr->setScale(0.8);

        m_fields->m_buttonMenu = CCMenuItemSpriteExtra::create(
            spr,
            nullptr,
            this,
            menu_selector(WREditLevelLayer::onLinkButton)
        );
        m_fields->m_buttonMenu->setID("wr-menu-button-edit");
        m_fields->m_buttonMenu->setZOrder(1);
        m_fields->m_buttonMenu->setVisible(true);

        auto folderMenu = getChildByID("folder-menu");
        folderMenu->addChild(m_fields->m_buttonMenu);
        folderMenu->updateLayout();

        Mod::get()->setSavedValue(
        m_fields->m_idCurrent + "-levelname",m_fields->m_nameCurrent);
        
        return true;
    }

    void onLinkButton (CCObject*) {
        WRMenu::create(m_fields->m_idCurrent
        , m_fields->m_nameCurrent
        , m_fields->m_idPrevious
        , m_fields->m_namePrevious)->show();
    }
};


