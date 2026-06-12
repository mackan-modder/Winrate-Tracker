



using namespace geode::prelude;

class WRMenu : public geode::Popup {
protected:
    std::string m_idCurrent = "1";
    std::string m_nameCurrent = "Stereo Madness";
    std::string m_idPrevious = "1";
    std::string m_namePrevious = "Stereo Madness";
    bool m_isLinked;
    CCMenuItemSpriteExtra* m_buttonLink = nullptr;
    CCMenuItemSpriteExtra* m_buttonUnLink = nullptr;
    CCMenuItemSpriteExtra* m_buttonLinkedList = nullptr;
    CCMenuItemSpriteExtra* m_buttonReset = nullptr;
    CCMenuItemSpriteExtra* m_buttonStats = nullptr;


    bool init();

    void onButtonLink(CCObject*);
    void unlinkPreviousPopup();
    void onUnLinkCurrent(CCObject*);
    void onLinkedList(CCObject*);
    void onResetWinrate(CCObject*);

    void linkPopup();
    void linkPopup2();
    void unlinkLevel(std::string level);
    void linkLevel(std::string levelKeep, std::string levelDicard);

public:
    static WRMenu* create(std::string idCurrent, std::string nameCurrent, std::string idPrevious, std::string namePrevious);

};