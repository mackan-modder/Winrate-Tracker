



using namespace geode::prelude;

class WRMenu : public geode::Popup {
protected:
    std::string m_idCurrent = "1";
    std::string m_nameCurrent = "Stereo Madness";
    std::string m_idPrevious = "1";
    std::string m_namePrevious = "Stereo Madness";
    CCMenuItemSpriteExtra* m_buttonLink = nullptr;
    CCMenuItemSpriteExtra* m_buttonReset = nullptr;
    CCMenuItemSpriteExtra* m_buttonStats = nullptr;


    bool init();
    void onLink(CCObject*);
    void onResetWinrate(CCObject*);
    void onStats(CCObject*);

public:
    static WRMenu* create(std::string idCurrent, std::string nameCurrent, std::string idPrevious, std::string namePrevious);

};