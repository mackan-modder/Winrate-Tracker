
#include <Geode/modify/PlayLayer.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include <Geode/utils/random.hpp>

static const float ALPHA = 0.3333;
static const double UPPERLIMIT = pow(10,200);
static const double LOWERLIMIT = pow(10,-6);
static const double THOUSAND = pow(10,3);
static const double MILLION = pow(10,6);
static const double BILLION = pow(10,9);
static const double TRILLION = pow(10,12);
static const double QUADRILLION = pow(10,15);
static const double QUINTILLION = pow(10,18);
static const double SEXTILLION = pow(10,21);
static const double SEPTILLION = pow(10,24);
static const double OCTILLION = pow(10,27);
static const double NONILLION = pow(10,30);


static const bool DO_DYNAMIC = true;



using namespace geode::prelude;

class $modify(WRPlayLayer, PlayLayer){
	struct Fields {
		std::array<float,100> m_percentageWinrate;
		std::array<float,100> m_tempWinrate;
        std::array<int,100> m_percentageDataCount;
		// m_linkedLevels includes the level we are in and other linked levels
		std::set<std::string> m_linkedLevels; 
		std::array<float,100> m_percentageTimeLength;
		std::string m_levelId;
		std::string m_levelName;
		bool m_scheduleLinkPopup = false;
		std::string m_linkLevelName;
		double m_currentWinrate;
		double m_currentTime;
		bool m_measuringTimeLength;
		float m_currentMeasurement;
		int m_currentIndex;
		bool m_currentIndexActive;
		float m_startingPercentage = -1;
		int m_endOfSafeZone = -1;
		CCNodeRGBA* m_parentContainer = nullptr;
		CCLabelBMFont* m_winrateLabel = nullptr;
		CCLabelBMFont* m_winrateLabelFlat = nullptr;
		std::string m_winrateLabelFlatString;
		CCLabelBMFont* m_completionTimeLabel = nullptr;
		std::string m_completionTimeLabelString;

		~Fields() {
			for (std::string levelId : m_linkedLevels) {
				Mod::get()->setSavedValue(levelId + "-winrate", m_percentageWinrate);
				Mod::get()->setSavedValue(levelId+ "-datacount", m_percentageDataCount);
				Mod::get()->setSavedValue(levelId + "-timelength", m_percentageTimeLength);
				Mod::get()->setSavedValue(levelId + "-linked", m_linkedLevels);
			}

			
        }
	};

	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        // log::info("init()");

		for(int i=0;i<100;i++){	
			m_fields->m_percentageWinrate[i] = 1.0;
			m_fields->m_percentageDataCount[i] = 0;
			m_fields->m_percentageTimeLength[i] = -1;
		}

		int ourLevelId = level->m_levelID.value();
		if (!ourLevelId) {
			ourLevelId = EditorIDs::getID(level);
		}

		std::string levelId = fmt::to_string(ourLevelId);

		// Saving the levelID to use when saving the data later

		m_fields->m_percentageWinrate = Mod::get()->getSavedValue<std::array<float,100>>(levelId + "-winrate", m_fields->m_percentageWinrate);
		m_fields->m_percentageDataCount = Mod::get()->getSavedValue<std::array<int,100>>(levelId+ "-datacount", m_fields->m_percentageDataCount);
		m_fields->m_percentageTimeLength = Mod::get()->getSavedValue<std::array<float,100>>(levelId + "-timelength", m_fields->m_percentageTimeLength);
		m_fields->m_linkedLevels = Mod::get()->getSavedValue<std::set<std::string>>(levelId + "-linked", m_fields->m_linkedLevels);

		m_fields->m_linkedLevels.insert(levelId);
		m_fields->m_levelId = levelId;


		// Saving the id in a list here we can loop through later
		auto dataAll = Mod::get()->getSavedValue<std::set<std::string>>("all-level-ids");
		dataAll.insert(levelId);
		Mod::get()->setSavedValue("all-level-ids", dataAll);

		m_fields->m_levelName = level->m_levelName;

		Mod::get()->setSavedValue(levelId + "-levelname",level->m_levelName);

		for (std::string id : dataAll) {
			log::info("{} 1 {}",id, Mod::get()->getSavedValue<std::string>(id + "-levelname", ""));
		}
		

		m_fields->m_measuringTimeLength = false;
		for(int i=0;i<100;i++){	
			if (m_fields->m_percentageTimeLength[i] == -1) {
				m_fields->m_measuringTimeLength = true;
				break;
			}
		}

		m_fields->m_parentContainer = CCNodeRGBA::create();
		m_fields->m_parentContainer->setCascadeColorEnabled(true);
		m_fields->m_parentContainer->setCascadeOpacityEnabled(true);

		m_fields->m_parentContainer->setLayout(
			ColumnLayout::create()
				->setGap(10.f)->setGrowCrossAxis(true)
				->setAxisAlignment(AxisAlignment::Start)->setCrossAxisLineAlignment(AxisAlignment::Start));

		auto uiLayer = this->getChildByID("UILayer");

		m_fields->m_parentContainer->setPosition(uiLayer->getPosition() + CCPoint { 10.0f, 10.0f });
		m_fields->m_parentContainer->setScale(0.35);
		m_fields->m_parentContainer->setContentHeight(700);
		m_fields->m_parentContainer->setContentWidth(700);

		uiLayer->addChild(m_fields->m_parentContainer);

		m_fields->m_parentContainer->setID("parent-label"_spr);

		if (Mod::get()->getSettingValue<bool>("winrate-dynamic")) {
			m_fields->m_winrateLabel = CCLabelBMFont::create("Winrate is TEST", "bigFont.fnt");

			m_fields->m_parentContainer->addChild(m_fields->m_winrateLabel);

			m_fields->m_winrateLabel->setAnchorPoint({0,0});
			
			m_fields->m_parentContainer->updateLayout();
		}
		if (Mod::get()->getSettingValue<bool>("winrate-flat")) {
			m_fields->m_winrateLabelFlat = CCLabelBMFont::create("Winrate is TEST", "bigFont.fnt");

			m_fields->m_parentContainer->addChild(m_fields->m_winrateLabelFlat);

			m_fields->m_winrateLabelFlat->setAnchorPoint({0,0});
			
			m_fields->m_parentContainer->updateLayout();
		}
		if (Mod::get()->getSettingValue<bool>("completion-time")) {
			m_fields->m_completionTimeLabel = CCLabelBMFont::create("Time from 0 to 100 is TEST", "bigFont.fnt");

			m_fields->m_parentContainer->addChild(m_fields->m_completionTimeLabel);

			m_fields->m_completionTimeLabel->setAnchorPoint({0,0});
		}
		
		m_fields->m_parentContainer->setOpacity(static_cast<int>(255.0*Mod::get()->getSettingValue<float>("label-opacity")));

		m_fields->m_parentContainer->updateLayout();

		updateStaticTextLabels();

		auto lastElementWithZeroData = std::find(m_fields->m_percentageDataCount.begin(),m_fields->m_percentageDataCount.end(),0);
		int lastIndexWinrate = lastElementWithZeroData-m_fields->m_percentageDataCount.begin()+1;
		m_fields->m_currentWinrate = calculateWinrate(0, lastIndexWinrate);

		auto lastValidIterator = std::find(m_fields->m_percentageTimeLength.begin(),m_fields->m_percentageTimeLength.end(),-1);
		int lastIndexTime = lastValidIterator-m_fields->m_percentageTimeLength.begin();
		m_fields->m_currentTime = calculateTimeToComplete(lastIndexTime); 


		if (m_fields->m_parentContainer) { // My way of checking for init()
			updateStaticTextLabels();
			m_fields->m_currentMeasurement = 0;
			m_fields->m_currentIndex = getCurrentPercentInt();
			m_fields->m_currentIndexActive = (getCurrentPercent()==0.0) ? true : false;
			m_fields->m_endOfSafeZone = -1;
			m_fields->m_startingPercentage = getCurrentPercentInt();
			log::info("reset {}",getCurrentPercentInt());
		}


		// Until I find the motivation to learn menus and 
		// and implement searching for levels to link, this is a fair solution
		// imo
		auto previousLevel = Mod::get()->getSavedValue<std::string>("previous-level", "");
		auto previousLevelName = Mod::get()->getSavedValue<std::string>(previousLevel+"-levelname", "");

		if (previousLevelName!=m_fields->m_levelName && previousLevelName != "" && m_fields->m_levelName != "") {
			int minimumLength = static_cast<int>(std::min({previousLevelName.length(),m_fields->m_levelName.length(),static_cast<unsigned long long>(5)}));

			if (previousLevelName.compare(0, minimumLength, previousLevelName)) {
				m_fields->m_scheduleLinkPopup = true;
				m_fields->m_linkLevelName = previousLevelName;
			}
		}


		Mod::get()->setSavedValue("previous-level", levelId);

		return true;
	}

	

	void levelComplete() {
		int startPercentage = (m_fields->m_startingPercentage==0.0) ? 0 : m_fields->m_endOfSafeZone;
		updateWinrate(startPercentage,100);
		updateChange();

		PlayLayer::levelComplete();
	}
	
	void resetLevel() {	

		// log::info("resetLevel()");

		PlayLayer::resetLevel();

		if (m_fields->m_measuringTimeLength) {
			m_fields->m_measuringTimeLength = false;
			for(int i=0;i<100;i++){	
				if (m_fields->m_percentageTimeLength[i] == -1) {
					m_fields->m_measuringTimeLength = true;
					break;
				}
			}
		}
		
		if (m_fields->m_parentContainer) { // My way of checking for init()
			updateStaticTextLabels();
			m_fields->m_currentMeasurement = 0;
			m_fields->m_currentIndex = getCurrentPercentInt();
			m_fields->m_currentIndexActive = (getCurrentPercent()==0.0) ? true : false;
			m_fields->m_endOfSafeZone = -1;
			m_fields->m_startingPercentage = getCurrentPercentInt();
			log::info("reset {}",getCurrentPercentInt());
		}
	}

    double calculateWinrate(int start, int end) {
        double product = 1;

        for(int i=start;i<std::min({end,100});i++){
            product *= static_cast<double>(m_fields->m_percentageWinrate[i]);
        }

        return product;
    }

	double calculateTimeToComplete(int end) {
        double expectedTime = 1; // We assume respawn time is 1 second

        for (int i=0;i<end;i++) {
			expectedTime = (static_cast<double>(m_fields->m_percentageTimeLength[i])+expectedTime)/std::max({static_cast<double>(m_fields->m_percentageWinrate[i]),LOWERLIMIT});
		}

        return expectedTime;
    }

	std::string assembleWinrateText(int firstIndex, int lastIndex, bool dynamic){
		std::string returnString = "";
		returnString += "Winrate";

		if (dynamic) returnString += " now" ;

		if (lastIndex<100) {
			returnString += " to " + std::to_string(std::max({0,lastIndex-1})) + "%";
		}

		returnString += ": ";

		if (getCurrentPercentInt()>=lastIndex) {
			return returnString += "?";
		}

		
		double interpolation 
		= (dynamic) ? (static_cast<double>(getCurrentPercent()) 
		- static_cast<double>(firstIndex)) : 0.0;

		double interpolatedWinrate 
		= calculateWinrate( firstIndex, lastIndex+1)*(1.0-interpolation)
		+ calculateWinrate( firstIndex+1, lastIndex+1)*interpolation;

		

		if (interpolatedWinrate==0.0) {
			return returnString += "1 in Infinity";
		} else if (interpolatedWinrate>0.1) {
			returnString += std::format("{:.3g}", interpolatedWinrate*100) + "%";
		} else {
			returnString += "1 in " + formatLargeNumbers((1.0/interpolatedWinrate));
		}

		return returnString;
	}

	std::string formatLargeNumbers(double number) {

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
		} else {
			return std::format("{:.3g}",number);
		}

		return "";
	}

	void updateWinrate(int start, int end) {
		
        log::info("updateWinrate({},{})",start,end);
		if (start==-1) {
			// log::info("SafeZoned!");
			return;
		}
		for(int i=start;i<end;i++){
			updateWinratePercentage(i,true,false);
		}

		
		if (end<100) {
            // log::info("index: {} old: {} new: {}",end,m_fields->m_percentageWinrate[end],m_fields->m_percentageWinrate[end]*(1-ALPHA));
			updateWinratePercentage(end, false,false);
		}
	}

    void updateWinratePercentage(int index, bool passed, bool temp) {
		// Initial higher alpha
        m_fields->m_percentageDataCount[index]++;
        float localAlpha = 2.0/(m_fields->m_percentageDataCount[index]*0.5+3.5);
        if (localAlpha<ALPHA) {
            localAlpha = ALPHA;
        }
        // log::info("index: {} old: {} new: {}",index,m_fields->m_percentageWinrate[index],localAlpha + m_fields->m_percentageWinrate[index]*(1-localAlpha));
		if (passed) {
			m_fields->m_percentageWinrate[index] = localAlpha + m_fields->m_percentageWinrate[index]*(1.0f-localAlpha);
		} else {
			m_fields->m_percentageWinrate[index] = m_fields->m_percentageWinrate[index]*(1.0f-localAlpha);
		}
        
    }

	void updateDynamicTextLabels() {
		auto lastElementWithZeroData = std::find(m_fields->m_percentageDataCount.begin(),m_fields->m_percentageDataCount.end(),0);
		int lastIndex = lastElementWithZeroData-m_fields->m_percentageDataCount.begin();
		m_fields->m_winrateLabel->setString((assembleWinrateText(getCurrentPercentInt(),lastIndex,true)).c_str());
		m_fields->m_parentContainer->updateLayout();

	}

	void updateStaticTextLabels() {
		auto lastElementWithZeroData = std::find(m_fields->m_percentageDataCount.begin(),m_fields->m_percentageDataCount.end(),0);
		int lastIndex = lastElementWithZeroData-m_fields->m_percentageDataCount.begin();

		if (m_fields->m_completionTimeLabel){
			m_fields->m_completionTimeLabelString = assembleCompletionTimeText();
			m_fields->m_completionTimeLabel->setString(m_fields->m_completionTimeLabelString.c_str());
		}

		if (m_fields->m_winrateLabelFlat){
			m_fields->m_winrateLabelFlatString = assembleWinrateText(0,lastIndex,false);
			m_fields->m_winrateLabelFlat->setString(m_fields->m_winrateLabelFlatString.c_str());
		}

		m_fields->m_parentContainer->updateLayout();
	}

	void updateChange() {
		// Calculating the new winrate
		auto lastElementWithZeroData = std::find(m_fields->m_percentageDataCount.begin(),m_fields->m_percentageDataCount.end(),0);
		int lastIndexWinrate = lastElementWithZeroData-m_fields->m_percentageDataCount.begin()+1;
		double newWinrate = calculateWinrate(0, lastIndexWinrate);

		
		// Calculating the new completion time
		auto lastValidIterator = std::find(m_fields->m_percentageTimeLength.begin(),m_fields->m_percentageTimeLength.end(),-1);
		int lastIndexTime = lastValidIterator-m_fields->m_percentageTimeLength.begin();
		double newTime = calculateTimeToComplete(lastIndexTime);

		double differenceTime = newTime-m_fields->m_currentTime;
		double differenceWinrate = newWinrate-m_fields->m_currentWinrate;

		if (m_fields->m_completionTimeLabel) {
			std::string changesTime = m_fields->m_completionTimeLabelString;
			
			changesTime += ((differenceWinrate<0) ? " (+" : " (-") + formatTime(std::abs(differenceTime)) + ")";
			m_fields->m_completionTimeLabel->setString(changesTime.c_str());
			
			m_fields->m_currentTime = newTime;

		}

		if (m_fields->m_winrateLabelFlat) {
			std::string changesWinrate = m_fields->m_winrateLabelFlatString;

			if (m_fields->m_currentWinrate>0.1) {
				changesWinrate += ((differenceWinrate>=0) ? " (+" : " (-") + formatLargeNumbers(std::abs(differenceWinrate*100)) + "%)";
			} else {
				double differenceWinrateInverse = 1/newWinrate-1/m_fields->m_currentWinrate;
				changesWinrate += ((differenceWinrate<0) ? " (+" : " (-") + formatLargeNumbers(std::abs(differenceWinrateInverse)) + ")";
			}

			m_fields->m_winrateLabelFlat->setString(changesWinrate.c_str());

			m_fields->m_currentWinrate = newWinrate;
		}
		m_fields->m_parentContainer->updateLayout();
	}	

	// void linkLevel(std::string levelKeep, std::string levelDicard) {
	// 	auto dataLevelKeep = Mod::get()->getSavedValue<std::set<std::string>>(levelKeep + "-linked");

	// 	auto dataLevelDicard = Mod::get()->getSavedValue<std::set<std::string>>(levelDicard + "-linked");

	// 	for (std::string id : dataLevelDicard) {
	// 		dataLevelKeep.insert(id);
	// 	}

	// 	Mod::get()->setSavedValue(levelKeep + "-linked", dataLevelKeep);
	// 	Mod::get()->setSavedValue(levelDicard + "-linked", dataLevelKeep);

	// 	if (m_fields->m_levelId==levelKeep || m_fields->m_levelId==levelDicard)	{
	// 		for (std::string id : dataLevelKeep) {
	// 			m_fields->m_linkedLevels.insert(id);
	// 		}
	// 	}

	// 	if (m_fields->m_levelId==levelDicard) {
	// 		m_fields->m_percentageWinrate = Mod::get()->getSavedValue<std::array<float,100>>(levelKeep + "-winrate", m_fields->m_percentageWinrate);
	// 		m_fields->m_percentageDataCount = Mod::get()->getSavedValue<std::array<int,100>>(levelKeep+ "-datacount", m_fields->m_percentageDataCount);
	// 		m_fields->m_percentageTimeLength = Mod::get()->getSavedValue<std::array<float,100>>(levelKeep + "-timelength", m_fields->m_percentageTimeLength);
	// 	}
	// }

	// ok so something is extremely wrong, there was no

	// void updateChangeDynamic() { // NOT FINISHED
	// 	// Calculating the new winrate
	// 	auto lastElementWithZeroData = std::find(m_fields->m_percentageDataCount.begin(),m_fields->m_percentageDataCount.end(),0);
	// 	int lastIndexWinrate = lastElementWithZeroData-m_fields->m_percentageDataCount.begin()+1;
	// 	double newWinrate = calculateWinrate(0, lastIndexWinrate);

		
	// 	// Calculating the new completion time
	// 	auto lastValidIterator = std::find(m_fields->m_percentageTimeLength.begin(),m_fields->m_percentageTimeLength.end(),-1);
	// 	int lastIndexTime = lastValidIterator-m_fields->m_percentageTimeLength.begin();
	// 	double newTime = calculateTimeToComplete(lastIndexTime);

	// 	double differenceTime = newTime-m_fields->m_currentTime;
	// 	double differenceWinrate = newWinrate-m_fields->m_currentWinrate;

	// 	if (m_fields->m_completionTimeLabel) {
	// 		std::string changesTime = m_fields->m_completionTimeLabelString;
			
	// 		changesTime += ((differenceWinrate<0) ? " (+" : " (-") + formatTime(std::abs(differenceTime)) + ")";
	// 		m_fields->m_completionTimeLabel->setString(changesTime.c_str());
			
	// 		m_fields->m_currentTime = newTime;

	// 	}

	// 	if (m_fields->m_completionTimeLabel) {
	// 		std::string changesWinrate = m_fields->m_winrateLabelFlatString;

	// 		if (m_fields->m_currentWinrate>0.1) {
	// 			changesWinrate += ((differenceWinrate>=0) ? " (+" : " (-") + formatLargeNumbers(std::abs(differenceWinrate*100)) + "%)";
	// 		} else {
	// 			double differenceWinrateInverse = 1/newWinrate-1/m_fields->m_currentWinrate;
	// 			changesWinrate += ((differenceWinrate<0) ? " (+" : " (-") + formatLargeNumbers(std::abs(differenceWinrateInverse)) + ")";
	// 		}

	// 		m_fields->m_winrateLabelFlat->setString(changesWinrate.c_str());

	// 		m_fields->m_currentWinrate = newWinrate;
	// 	}
	// 	m_fields->m_parentContainer->updateLayout();
	// }

	// double calculateWinrateDynamic() {
	// 	log::info(" ");
	// 	return 1;
	// }

	std::string assembleCompletionTimeText() {
		std::string returnString = "Time for ";

		auto lastValidIterator = std::find(m_fields->m_percentageTimeLength.begin(),m_fields->m_percentageTimeLength.end(),-1);
		int lastIndex = lastValidIterator-m_fields->m_percentageTimeLength.begin();

		returnString += fmt::to_string(lastIndex) + "%: ";

		double time = calculateTimeToComplete(lastIndex);

		returnString += (time != 1.0) ? formatTime(time) : "?";

		return returnString;
	}

	std::string formatTime(double time) {
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

	void postUpdate(float dt) {

			
		

		if (m_fields->m_winrateLabel) updateDynamicTextLabels();

		

		if (m_fields->m_measuringTimeLength){
			measureTimeUpdate(dt);
		}

		// Logic for adding a safe region in the beginning of each attempt 
		// from practice mode or startpos.
		if (m_fields->m_endOfSafeZone == -1 && m_attemptTime>1.2) {
			m_fields->m_endOfSafeZone = getCurrentPercentInt()+1;
		}

		// if (DO_DYNAMIC && !m_player1->m_isDead && getCurrentPercentInt()>=m_fields->m_endOfSafeZone) {
		// 	updateChangeDynamic();
		// }
		
		PlayLayer::postUpdate(dt);
	}

	void measureTimeUpdate(float dt) {
		if (getCurrentPercent()!=0.0) m_fields->m_currentMeasurement += dt;
			if (m_fields->m_currentIndex!=getCurrentPercentInt()) {
				// log::info("measurement: {} {} {}",m_fields->m_currentMeasurement,m_fields->m_currentIndexActive, m_fields->m_percentageTimeLength[m_fields->m_currentIndex]);
				if (m_fields->m_currentIndexActive && m_fields->m_percentageTimeLength[m_fields->m_currentIndex]==-1 && getCurrentPercentInt()==m_fields->m_currentIndex+1) {
					m_fields->m_percentageTimeLength[m_fields->m_currentIndex] = m_fields->m_currentMeasurement;

					// log::info("Updated");

					m_fields->m_measuringTimeLength = false;
					for(int i=0;i<100;i++){	
						if (m_fields->m_percentageTimeLength[i] == -1) {
							m_fields->m_measuringTimeLength = true;
						}
					}
				} 

				m_fields->m_currentIndexActive = true;
				m_fields->m_currentIndex=getCurrentPercentInt();
				m_fields->m_currentMeasurement = 0.0;
			}
	}
};