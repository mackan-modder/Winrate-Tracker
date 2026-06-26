



using namespace geode::prelude;

class WRMenu : public geode::Popup {
protected:
    gd::string m_idCurrent = "1";
    gd::string m_nameCurrent = "Stereo Madness";
    gd::string m_idPrevious = "1";
    gd::string m_namePrevious = "Stereo Madness";
    CCMenuItemSpriteExtra* m_buttonLink = nullptr;
    CCMenuItemSpriteExtra* m_buttonReset = nullptr;
    CCMenuItemSpriteExtra* m_buttonStats = nullptr;


    bool init();
    void onLink(CCObject*);
    void onResetWinrate(CCObject*);
    void onStats(CCObject*);

public:
    static WRMenu* create(gd::string idCurrent, gd::string nameCurrent, gd::string idPrevious, gd::string namePrevious);

};