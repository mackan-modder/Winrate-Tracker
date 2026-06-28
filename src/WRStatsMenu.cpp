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
    myMenuContent->setLayout(ColumnLayout::create()
    ->setAxisReverse(true)->setAxisAlignment(AxisAlignment::End));
    myMenuContent->setScale(0.6);

    auto background 
    = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
    background->setColor({ 0, 0, 0 });
    background->setOpacity(100);
    background->setContentSize({ 225.f, 85.f });
    background->setID("WR-background");

    auto spriteOverall 
    = ButtonSprite::create("Overall Stats");

    m_buttonOverall = CCMenuItemSpriteExtra::create(
        spriteOverall,
        nullptr,
        this,
        menu_selector(WRStatsMenu::onOverall)
    );
    m_buttonOverall->setID("wr-button-Overall");
    m_buttonOverall->setZOrder(1);
    m_buttonOverall->setScale(2);
    m_buttonOverall->setVisible(true);
    myMenuContent->addChildAtPosition(
    m_buttonOverall,Anchor::Top);

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

    auto spriteImpact 
    = ButtonSprite::create("Possible Improvements");

    m_buttonImpact = CCMenuItemSpriteExtra::create(
        spriteImpact,
        nullptr,
        this,
        menu_selector(WRStatsMenu::onImpact)
    );
    m_buttonImpact->setID("wr-button-improvements");
    m_buttonImpact->setZOrder(1);
    m_buttonImpact->setScale(2);
    m_buttonImpact->setVisible(true);
    myMenuContent->addChildAtPosition(m_buttonImpact,Anchor::Top);

    myMenuContent->updateLayout();

    CCPoint offset = {0,-75};
    CCPoint offsetBackground = {0,-10};
    m_mainLayer->addChildAtPosition(
    background,Anchor::Center,offsetBackground);
    m_mainLayer->addChildAtPosition(
    myMenuContent, Anchor::Center, offset);
    
    m_mainLayer->updateLayout();

    return true;
}


WRStatsMenu* WRStatsMenu::create(std::string idCur, std::string nameCur) {
    auto ret = new WRStatsMenu();
    ret->m_idCurrent = idCur;
    ret->m_nameCurrent = nameCur;
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

    winrate = Mod::get()->getSavedValue<std::array<float,100>>(
    m_idCurrent+"-winrate", winrate);
    times = Mod::get()->getSavedValue<std::array<float,100>>(
    m_idCurrent+"-timelength", times);

    std::string levelsString = "Possible \"Time for 100\%\" to remove:\n";


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
        levelsString += fmt::to_string(i*10) 
        + "%-" + fmt::to_string(i*10+10) + "%: ";
        
        
        double expectedTimeChanged = 1; // We assume respawn time is 1 second

        for (int j=0;j<100;j++) {
            if (j>=i*10 && j<(i+1)*10) {
                expectedTimeChanged 
                = (static_cast<double>(times[j])+expectedTimeChanged);
            } else {
                expectedTimeChanged 
                = (static_cast<double>(times[j])+expectedTimeChanged)
                /std::max(
                {static_cast<double>(winrate[j]),LOWERLIMIT});
            }
        }
        
        double impact = std::abs(expectedTimeNormal-expectedTimeChanged);
        
        levelsString += fmt::format("{:.1f}",impact/expectedTimeNormal*100) 
        + "\%";
        levelsString += "\n";
    }

    geode::createQuickPopup(
        "Possible Time Improvement",            // title
        levelsString,   // content
        "Back", "Copy",      // buttons
        [levelsString](auto, bool btn2) {
            if (btn2) {
                Notification::create(
                "Copied Stats to Clipboard!",
                NotificationIcon::Success, 5.0f)
                ->show();
                clipboard::write(levelsString);
                // say hi to mom
            }
        }
    );

    return;
}

void WRStatsMenu::onPass(CCObject*) {
    std::array<float,100> winrate;
    std::array<int,100> dataCount;
    for(int i=0;i<100;i++){	
        winrate[i] = 1.0;
        dataCount[i] = 0;
    }

    winrate = Mod::get()->getSavedValue<std::array<float,100>>
    (m_idCurrent+"-winrate", winrate);

    dataCount = Mod::get()->getSavedValue<std::array<int,100>>
    (m_idCurrent+"-datacount", dataCount);

    std::string levelsString = m_nameCurrent + " has passrates:\n";

    for (int i = 0;i<10;i++) {
        levelsString += fmt::to_string(i*10) + "%-" 
        + fmt::to_string(i*10+10) + "%: ";
        double product = 1;

        bool noData = false;
        for (int j = 0;j<10;j++) {
            if (dataCount[i*10+j]==0) noData = true;
        }
        if (noData) {
            levelsString += "-\n";
            continue;
        }

        for (int j = 0;j<10;j++) {
            product *= static_cast<double>(winrate[i*10+j]);
        }

        if (product<0.01 && product!=0) {
            levelsString += "1 in " + formatLargeNumbers(1/product);
        } else {
            levelsString += fmt::format("{:.1f}",product*100) + "%";
        }
        levelsString += "\n";
    }
    
    // FLAlertLayer::create(
    // "Level Stats",    // title
    // levelsString,  // content
    // "OK"        // button
    // )->show();

    geode::createQuickPopup(
        "Level Passrates",            // title
        levelsString,   // content
        "Back", "Copy",      // buttons
        [levelsString](auto, bool btn2) {
            if (btn2) {
                Notification::create(
                "Copied Stats to Clipboard!",
                NotificationIcon::Success, 5.0f)
                ->show();
                clipboard::write(levelsString);
                // say hi to mom
            }
        }
    );

    return;
}

void WRStatsMenu::onOverall(CCObject*) {

    
    std::array<float,100> times;
    std::array<float,100> winrate;
    std::array<int,100> dataCount;
    for(int i=0;i<100;i++){	
        winrate[i] = 1.0;
        dataCount[i] = 0;
        times[i] = -1;
    }

    winrate = Mod::get()->getSavedValue<std::array<float,100>>
    (m_idCurrent+"-winrate", winrate);

    dataCount = Mod::get()->getSavedValue<std::array<int,100>>
    (m_idCurrent+"-datacount", dataCount);

    times = Mod::get()->getSavedValue<std::array<float,100>>
    (m_idCurrent+"-timelength", times);

    std::set<std::string> linkedList 
    = Mod::get()->getSavedValue<std::set<std::string>>(m_idCurrent+"-linked");

    int totalAttempts = 0;

    for (std::string levelId : linkedList) {
        totalAttempts += Mod::get()->getSavedValue<int>(levelId+"-attempts");
    }
    
    double totalwinrate = 1;
    double timeFor100 = 1;
    bool hasData = true; 
    for (int i = 0;i<100;i++) {
        if (dataCount[i]==0 || times[i]==-1) hasData = false;
        totalwinrate *= winrate[i];
        timeFor100 = (static_cast<double>(times[i])+timeFor100)
        /std::max({static_cast<double>(winrate[i]),LOWERLIMIT});
    }

    std::string StatsString = "";

    if (hasData) {
        
        time_t timestamp = time(nullptr);
        tm tm = geode::localtime(timestamp);
        StatsString += fmt::to_string(tm.tm_year+1900) + "-";
        if (tm.tm_mon<10) StatsString += "0";
        StatsString += fmt::to_string(tm.tm_mon) + "-"; 
        if (tm.tm_mday<10) StatsString += "0";
        StatsString += fmt::to_string(tm.tm_mday) + " ";
        if (tm.tm_hour<10) StatsString += "0";
        StatsString += fmt::to_string(tm.tm_hour) + ":"; 
        if (tm.tm_min<10) StatsString += "0";
        StatsString += fmt::to_string(tm.tm_min) + "\n";

        StatsString += fmt::to_string(totalAttempts) + " attempts\n";

        StatsString += "Winrate: ";

        if (totalwinrate<0.01 && totalwinrate!=0) {
            StatsString += "1 in " + formatLargeNumbers(1/totalwinrate);
        } else {
            StatsString += fmt::format("{:.1f}",totalwinrate*100) + "%";
        }
        StatsString += "\n";


        StatsString += "Time for 100%: " + formatTime(timeFor100);

        
        

    } else {
        StatsString += "Not enough data." 
        "\nComplete every percentage in the level atleast once.";
    }

    geode::createQuickPopup(
        "Overall Stats",            // title
        StatsString,   // content
        "Back", "Copy",      // buttons
        [StatsString](auto, bool btn2) {
            if (btn2) {
                Notification::create(
                "Copied Stats to Clipboard!",
                NotificationIcon::Success, 5.0f)
                ->show();
                clipboard::write(StatsString);
                // say hi to mom
            }
        }
    );


    return;
}

std::string WRStatsMenu::formatLargeNumbers(double number) {
    if (number<1000.0) {
        return fmt::format("{:.3g}",number);
    } else if (number<MILLION) {
        number /= THOUSAND;
        return fmt::format("{:.3g}",number) + " thousand";
    } else if (number<BILLION) {
        number /= MILLION;
        return fmt::format("{:.3g}",number) + " million";
    } else if (number<TRILLION) {
        number /= BILLION;
        return fmt::format("{:.3g}",number) + " billion";
    } else if (number<QUADRILLION) {
        number /= TRILLION;
        return fmt::format("{:.3g}",number) + " trillion";
    } else if (number<QUINTILLION) {
        number /= QUADRILLION;
        return fmt::format("{:.3g}",number) + " quadrillion";
    } else if (number<SEXTILLION) {
        number /= QUINTILLION;
        return fmt::format("{:.3g}",number) + " quintillion";
    } else if (number<SEPTILLION) {
        number /= SEXTILLION;
        return fmt::format("{:.3g}",number) + " sextillion";
    } else if (number<OCTILLION) {
        number /= SEPTILLION;
        return fmt::format("{:.3g}",number) + " septillion";
    } else if (number<NONILLION) {
        number /= OCTILLION;
        return fmt::format("{:.3g}",number) + " octillion";
    } else if (number<DECILLION) {
        number /= NONILLION;
        return fmt::format("{:.3g}",number) + " nonillion";
    } else if (number<UNDECILLION) {
        number /= DECILLION;
        return fmt::format("{:.3g}",number) + " decillion";
    } else if (number<DUODECILLION) {
        number /= UNDECILLION;
        return fmt::format("{:.3g}",number) + " undecillion";
    } else if (number<TREDECILLION) {
        number /= DUODECILLION;
        return fmt::format("{:.3g}",number) + " duodecillion";
    } else if (number<QUATTUORDECILLION) {
        number /= TREDECILLION;
        return fmt::format("{:.3g}",number) + " tredecillion";
    } else if (number<QUINDECILLION) {
        number /= QUATTUORDECILLION;
        return fmt::format("{:.3g}",number) + " quattuordecillion";
    } else if (number<SEXDECILLION) {
        number /= QUINDECILLION;
        return fmt::format("{:.3g}",number) + " quindecillion";
    } else if (number<SEPTENDECILLION) {
        number /= SEXDECILLION;
        return fmt::format("{:.3g}",number) + " sexdecillion";
    } else if (number<OCTODECILLION) {
        number /= SEPTENDECILLION;
        return fmt::format("{:.3g}",number) + " septendecillion";
    } else if (number<NOVEMDECILLION) {
        number /= OCTODECILLION;
        return fmt::format("{:.3g}",number) + " octodecillion";
    } else if (number<VIGINTILLION) {
        number /= NOVEMDECILLION;
        return fmt::format("{:.3g}",number) + " novemdecillion";
    } else {
        return fmt::format("{:.3g}",number) ;
    }

    return "";
}

std::string WRStatsMenu::formatTime(double time) {
		std::string timeUnit;
		std::string returnNumberString;
		std::string timeString;

		if (Mod::get()->getSettingValue<bool>("time-format")) {
			if (time>60*60*24*7*52) {
				time /= 60*60*24*7*52;
				timeString = formatLargeNumbers(time);
				timeUnit = "years";
			} else if (time>60*60*24) {
				time /= 60*60*24;
				timeString = fmt::format("{:.3g}",time);
				timeUnit = "days";
			} else if (time>60*60) {
				time /= 60*60;
				timeString = fmt::format("{:.3g}",time);
				timeUnit = "hours";
			} else if (time>60) {
				time /= 60;
				timeString = fmt::format("{:.3g}",time);
				timeUnit = "minutes";
			} else {
				timeString = fmt::format("{:.3g}",time);
				timeUnit = "seconds";
			}	
		} else {
			if (time>60*60) {
				time /= 60*60;
				timeString = fmt::format("{:.3g}",time);
				timeUnit = "hours";
			} else if (time>60) {
				time /= 60;
				timeString = fmt::format("{:.3g}",time);
				timeUnit = "minutes";
			} else {
				timeString = fmt::format("{:.3g}",time);
				timeUnit = "seconds";
			}
		}
		
		return timeString + " " + timeUnit;
	}