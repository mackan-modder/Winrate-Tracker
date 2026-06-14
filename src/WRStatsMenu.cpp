#include "WRStatsMenu.hpp"
#include "WRLinkMenu.hpp"
#include "constants.cpp"

using namespace geode::prelude;

bool WRStatsMenu::init() {

    if (!Popup::init(250,130)) {
        return false;
    }

    this->setTitle("Winrate Stats Menu");
    this->setID("WR-Stats-Menu"_spr);

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

    auto spritePass = ButtonSprite::create("Passrates");
    m_buttonPass = CCMenuItemSpriteExtra::create(
        spritePass,
        nullptr,
        this,
        menu_selector(WRStatsMenu::onPass)
    );
    m_buttonPass->setID("wr-button-Pass");
    m_buttonPass->setZOrder(1);
    m_buttonPass->setScale(2);
    m_buttonPass->setVisible(true);
    myMenuContent->addChildAtPosition(m_buttonPass,Anchor::Top);


    // auto spriteStats = ButtonSprite::create("Passrate Impact");
    // m_buttonStats = CCMenuItemSpriteExtra::create(
    //     spriteStats,
    //     nullptr,
    //     this,
    //     menu_selector(WRStatsMenu::onStats)
    // );
    // m_buttonStats->setID("wr-button-stats");
    // m_buttonStats->setZOrder(1);
    // m_buttonStats->setScale(2);
    // m_buttonStats->setVisible(true);
    // myMenuContent->addChildAtPosition(m_buttonStats,Anchor::Top);

    auto spriteImpact = ButtonSprite::create("Passrate Times");
    m_buttonImpact = CCMenuItemSpriteExtra::create(
        spriteImpact,
        nullptr,
        this,
        menu_selector(WRStatsMenu::onImpact)
    );
    m_buttonImpact->setID("wr-button-Impact");
    m_buttonImpact->setZOrder(1);
    m_buttonImpact->setScale(2);
    m_buttonImpact->setVisible(true);
    myMenuContent->addChildAtPosition(m_buttonImpact,Anchor::Top);

    myMenuContent->updateLayout();

    CCPoint offset = {0,-75};
    CCPoint offsetBackground = {0,-10};
    m_mainLayer->addChildAtPosition(background,Anchor::Center,offsetBackground);
    m_mainLayer->addChildAtPosition(myMenuContent, Anchor::Center, offset);
    
    m_mainLayer->updateLayout();

    return true;
}


WRStatsMenu* WRStatsMenu::create(std::string idCurrent, std::string nameCurrent) {
    auto ret = new WRStatsMenu();
    ret->m_idCurrent = idCurrent;
    ret->m_nameCurrent = nameCurrent;
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void WRStatsMenu::onImpact(CCObject*) {
    std::array<float,100> winrate;
    std::array<float,100> times;


    for(int i=0;i<100;i++){	
        winrate[i] = 1.0;
        times[i] = -1;
    }

    winrate = Mod::get()->getSavedValue<std::array<float,100>>(m_idCurrent+"-winrate", winrate);
    times = Mod::get()->getSavedValue<std::array<float,100>>(m_idCurrent+"-timelength", times);

    std::string levelsString = "Possible time to remove:\n";


    if (std::find(times.begin(), times.end(), -1) != times.end()) {

        levelsString += "Finish the level to get this metric!";

        FLAlertLayer::create(
        "Level Stats",    // title
        levelsString,  // content
        "OK"        // button
        )->show();

        return;
    }

    double expectedTimeNormal = 1; // We assume respawn time is 1 second

    for (int i=0;i<100;i++) {
        expectedTimeNormal = (static_cast<double>(times[i])+expectedTimeNormal)
        /std::max({static_cast<double>(winrate[i]),LOWERLIMIT});
    }

    log::info("{}",expectedTimeNormal);
    

    for (int i = 0;i<10;i++) {
        levelsString += std::to_string(i*10) + "%-" + std::to_string(i*10+10) + "%: ";
        
        
        double expectedTimeChanged = 1; // We assume respawn time is 1 second

        for (int j=0;j<100;j++) {
            if (j>=i*10 && j<(i+1)*10) {
                expectedTimeChanged = (static_cast<double>(times[j])+expectedTimeChanged);
            } else {
                expectedTimeChanged = (static_cast<double>(times[j])+expectedTimeChanged)
                /std::max({static_cast<double>(winrate[j]),LOWERLIMIT});
            }
        }
        
        double impact = std::abs(expectedTimeNormal-expectedTimeChanged);
        
        
        // levelsString += formatTime(impact) + " / " + std::format("{:.2f}",impact/expectedTimeNormal*100) + "\%";
        levelsString += std::format("{:.2f}",impact/expectedTimeNormal*100) + "\%";
        levelsString += "\n";
    }
    
    FLAlertLayer::create(
    "Level Stats",    // title
    levelsString,  // content
    "OK"        // button
    )->show();

    return;
}

void WRStatsMenu::onPass(CCObject*) {
    std::array<float,100> winrate;
    for(int i=0;i<100;i++){	
        winrate[i] = 1.0;
    }

    winrate = Mod::get()->getSavedValue<std::array<float,100>>(m_idCurrent+"-winrate", winrate);

    std::string levelsString = m_nameCurrent + " has passrates:\n";

    for (int i = 0;i<10;i++) {
        levelsString += std::to_string(i*10) + "%-" + std::to_string(i*10+10) + "%: ";
        double product = 1;
        for (int j = 0;j<10;j++) {
            product *= static_cast<double>(winrate[i*10+j]);
        }
        if (product<0.01 && product!=0) {
            levelsString += "1 in " + formatLargeNumbers(1/product);
        } else {
            levelsString += std::format("{:.2g}",product*100) + "%";
        }
        levelsString += "\n";
    }
    
    FLAlertLayer::create(
    "Level Stats",    // title
    levelsString,  // content
    "OK"        // button
    )->show();

    return;
}

void WRStatsMenu::onStats(CCObject*) {
    // IDK what I should put in here yet tbh

    return;
}

std::string WRStatsMenu::formatLargeNumbers(double number) {
    if (number<1000.0) {
        return std::format("{:.3g}",number);
    } else if (number<MILLION) {
        number /= THOUSAND;
        return std::format("{:.3g}",number) + " thousand";
    } else if (number<BILLION) {
        number /= MILLION;
        return std::format("{:.3g}",number) + " million";
    } else if (number<TRILLION) {
        number /= BILLION;
        return std::format("{:.3g}",number) + " billion";
    } else if (number<QUADRILLION) {
        number /= TRILLION;
        return std::format("{:.3g}",number) + " trillion";
    } else if (number<QUINTILLION) {
        number /= QUADRILLION;
        return std::format("{:.3g}",number) + " quadrillion";
    } else if (number<SEXTILLION) {
        number /= QUINTILLION;
        return std::format("{:.3g}",number) + " quintillion";
    } else if (number<SEPTILLION) {
        number /= SEXTILLION;
        return std::format("{:.3g}",number) + " sextillion";
    } else if (number<OCTILLION) {
        number /= SEPTILLION;
        return std::format("{:.3g}",number) + " septillion";
    } else if (number<NONILLION) {
        number /= OCTILLION;
        return std::format("{:.3g}",number) + " octillion";
    } else if (number<DECILLION) {
        number /= NONILLION;
        return std::format("{:.3g}",number) + " nonillion";
    } else if (number<UNDECILLION) {
        number /= DECILLION;
        return std::format("{:.3g}",number) + " decillion";
    } else if (number<DUODECILLION) {
        number /= UNDECILLION;
        return std::format("{:.3g}",number) + " undecillion";
    } else if (number<TREDECILLION) {
        number /= DUODECILLION;
        return std::format("{:.3g}",number) + " duodecillion";
    } else if (number<QUATTUORDECILLION) {
        number /= TREDECILLION;
        return std::format("{:.3g}",number) + " tredecillion";
    } else if (number<QUINDECILLION) {
        number /= QUATTUORDECILLION;
        return std::format("{:.3g}",number) + " quattuordecillion";
    } else if (number<SEXDECILLION) {
        number /= QUINDECILLION;
        return std::format("{:.3g}",number) + " quindecillion";
    } else if (number<SEPTENDECILLION) {
        number /= SEXDECILLION;
        return std::format("{:.3g}",number) + " sexdecillion";
    } else if (number<OCTODECILLION) {
        number /= SEPTENDECILLION;
        return std::format("{:.3g}",number) + " septendecillion";
    } else if (number<NOVEMDECILLION) {
        number /= OCTODECILLION;
        return std::format("{:.3g}",number) + " octodecillion";
    } else if (number<VIGINTILLION) {
        number /= NOVEMDECILLION;
        return std::format("{:.3g}",number) + " novemdecillion";
    } else {
        return std::format("{:.3g}",number) ;
    }

    return "";
}

std::string WRStatsMenu::formatTime(double time) {
    std::string timeUnit;
    std::string returnNumberString;
    std::string timeString;

    if (time>60*60*24*7*52) {
        time /= 60*60*24*7*52;
        timeString = formatLargeNumbers(time);
        timeUnit = "years";
    } else if (time>60*60*24) {
        time /= 60*60*24;
        timeString = std::format("{:.3g}",time);
        timeUnit = "days";
    } else if (time>60*60) {
        time /= 60*60;
        timeString = std::format("{:.3g}",time);
        timeUnit = "hours";
    } else if (time>60) {
        time /= 60;
        timeString = std::format("{:.3g}",time);
        timeUnit = "minutes";
    } else {
        timeString = std::format("{:.3g}",time);
        timeUnit = "seconds";
    }
    
    return timeString + " " + timeUnit;
}