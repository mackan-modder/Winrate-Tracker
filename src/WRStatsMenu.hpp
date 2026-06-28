



using namespace geode::prelude;

class WRStatsMenu : public geode::Popup {
protected:
    std::string m_idCurrent = "1";
    std::string m_nameCurrent = "Stereo Madness";
    std::string m_idPrevious = "1";
    std::string m_namePrevious = "Stereo Madness";
    CCMenuItemSpriteExtra* m_buttonPass = nullptr;
    CCMenuItemSpriteExtra* m_buttonImpact = nullptr;
    CCMenuItemSpriteExtra* m_buttonStats = nullptr;
    CCMenuItemSpriteExtra* m_buttonOverall = nullptr;


    bool init();
    void onPass(CCObject*);
    void onImpact(CCObject*);
    void onOverall(CCObject*);

    std::string formatLargeNumbers(double number);
    std::string formatTime(double time);

public:
    static WRStatsMenu* create(std::string idCurrent, std::string nameCurrent);

};