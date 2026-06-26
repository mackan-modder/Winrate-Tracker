



using namespace geode::prelude;

class WRLinkMenu : public geode::Popup {
protected:
    gd::string m_idCurrent = "1";
    gd::string m_nameCurrent = "Stereo Madness";
    gd::string m_idPrevious = "1";
    gd::string m_namePrevious = "Stereo Madness";
    bool m_isLinked;
    CCMenuItemSpriteExtra* m_buttonLink = nullptr;
    CCMenuItemSpriteExtra* m_buttonUnLink = nullptr;
    CCMenuItemSpriteExtra* m_buttonLinkedList = nullptr;


    bool init();

    void onButtonLink(CCObject*);
    void unlinkPreviousPopup();
    void onUnLinkCurrent(CCObject*);
    void onLinkedList(CCObject*);
    void onLink(CCObject*);
    void onResetWinrate(CCObject*);

    void linkPopup();
    void linkPopup2();
    void unlinkLevel(gd::string level);
    void linkLevel(gd::string levelKeep, gd::string levelDicard);

public:
    static WRLinkMenu* create(gd::string idCurrent, gd::string nameCurrent, gd::string idPrevious, gd::string namePrevious);

};