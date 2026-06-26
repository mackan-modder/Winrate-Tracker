



using namespace geode::prelude;

class WRStatsMenu : public geode::Popup {
protected:
    gd::string m_idCurrent = "1";
    gd::string m_nameCurrent = "Stereo Madness";
    gd::string m_idPrevious = "1";
    gd::string m_namePrevious = "Stereo Madness";
    CCMenuItemSpriteExtra* m_buttonPass = nullptr;
    CCMenuItemSpriteExtra* m_buttonImpact = nullptr;
    CCMenuItemSpriteExtra* m_buttonStats = nullptr;


    bool init();
    void onPass(CCObject*);
    void onImpact(CCObject*);
    void onStats(CCObject*);

    gd::string formatLargeNumbers(double number);
    gd::string formatTime(double time);

public:
    static WRStatsMenu* create(gd::string idCurrent, gd::string nameCurrent);

};