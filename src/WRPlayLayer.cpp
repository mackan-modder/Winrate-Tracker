
#include <Geode/modify/PlayLayer.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include <Geode/utils/random.hpp>

const float ALPHA = 0.25;

using namespace geode::prelude;

class $modify(WRPlayLayer, PlayLayer){
	struct Fields {
		std::array<float,100> m_percentageWinrate;
        std::array<int,100> m_percentageDataCount;
		std::array<float,100> m_percentageTimeLength;
		bool m_measuringTimeLength;
		float m_currentMeasurement;
		int m_currentIndex;
		bool m_currentIndexActive;

		float m_startingPercentage;
		int m_endOfSafeZone;
		std::string m_levelStringWinrate;
		std::string m_levelStringDataCount;
		std::string m_levelStringTimeLength;
		CCNodeRGBA* m_parentContainer = nullptr;
		CCLabelBMFont* m_winrateLabel = nullptr;
		CCLabelBMFont* m_winrateLabelFlat = nullptr;
		CCLabelBMFont* m_completionTimeLabel = nullptr;

		~Fields() {
			Mod::get()->setSavedValue(m_levelStringWinrate, m_percentageWinrate);
			Mod::get()->setSavedValue(m_levelStringDataCount, m_percentageDataCount);
			Mod::get()->setSavedValue(m_levelStringTimeLength, m_percentageTimeLength);
        }
	};

	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        log::info("init()");

		for(int i=0;i<100;i++){	
			m_fields->m_percentageWinrate[i] = 1.0;
			m_fields->m_percentageDataCount[i] = 0;
			m_fields->m_percentageTimeLength[i] = -1;
		}

		int ourLevelId = level->m_levelID.value();
		if (!ourLevelId) {
			ourLevelId = EditorIDs::getID(level);
		}

		m_fields->m_levelStringWinrate = fmt::to_string(ourLevelId).append("-winrate");
		m_fields->m_levelStringDataCount = fmt::to_string(ourLevelId).append("-datacount");
		m_fields->m_levelStringTimeLength = fmt::to_string(ourLevelId).append("-timelength");


		auto data1 = Mod::get()->getSavedValue<std::array<float,100>>(m_fields->m_levelStringWinrate, m_fields->m_percentageWinrate);
		auto data2 = Mod::get()->getSavedValue<std::array<int,100>>(m_fields->m_levelStringDataCount, m_fields->m_percentageDataCount);
		auto data3 = Mod::get()->getSavedValue<std::array<float,100>>(m_fields->m_levelStringTimeLength, m_fields->m_percentageTimeLength);

		memcpy(&m_fields->m_percentageWinrate, &data1, 100*sizeof(float));
		memcpy(&m_fields->m_percentageDataCount, &data2, 100*sizeof(int));
		memcpy(&m_fields->m_percentageTimeLength, &data3, 100*sizeof(float));

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

		m_fields->m_winrateLabel = CCLabelBMFont::create("Winrate is TEST", "bigFont.fnt");

		m_fields->m_parentContainer->addChild(m_fields->m_winrateLabel);

		m_fields->m_winrateLabel->setAnchorPoint({0,0});
		
		m_fields->m_parentContainer->updateLayout();

		m_fields->m_winrateLabelFlat = CCLabelBMFont::create("Winrate is TEST", "bigFont.fnt");

		m_fields->m_parentContainer->addChild(m_fields->m_winrateLabelFlat);

		m_fields->m_winrateLabelFlat->setAnchorPoint({0,0});
		
		m_fields->m_parentContainer->updateLayout();

		m_fields->m_completionTimeLabel = CCLabelBMFont::create("Time from 0 to 100 is TEST", "bigFont.fnt");

		m_fields->m_parentContainer->addChild(m_fields->m_completionTimeLabel);

		m_fields->m_completionTimeLabel->setAnchorPoint({0,0});

		m_fields->m_parentContainer->setOpacity(86);

		m_fields->m_parentContainer->updateLayout();

		updateStaticTextLabels();

		return true;
	}

	void levelComplete() {
		int startPercentage = (m_fields->m_startingPercentage==0.0) ? 0 : m_fields->m_endOfSafeZone;
		updateWinrate(startPercentage,100);
		updateStaticTextLabels();

		PlayLayer::levelComplete();
	}
	
	void resetLevel() {	

		log::info("resetLevel()");

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
		
		
		m_fields->m_currentMeasurement = 0;
		m_fields->m_currentIndex = getCurrentPercentInt();
		m_fields->m_currentIndexActive = (getCurrentPercent()==0.0) ? true : false;
		m_fields->m_endOfSafeZone = -1;
		m_fields->m_startingPercentage = getCurrentPercent();
	}

    float calculateWinrate(int start, int end) {
        float product = 1;

        for(int i=start;i<std::min({end,100});i++){
            product *= static_cast<float>(m_fields->m_percentageWinrate[i]);
        }

        return product;
    }

	float calculateTimeToComplete(int end) {
        float expectedTime = 1; // We assume respawn time is 1 second

        for (int i=0;i<end;i++) {
			expectedTime = (m_fields->m_percentageTimeLength[i]+expectedTime)/m_fields->m_percentageWinrate[i];
		}

        return expectedTime;
    }

	std::string assembleWinrateText(int firstIndex, int lastIndex, bool dynamic){
		std::string returnString = "";
		returnString += "Winrate";

		// if (dynamic) returnString += " from " + std::to_string(getCurrentPercentInt());
		if (dynamic) returnString += " now" ;

		if (lastIndex<99) {
			returnString += " to " + std::to_string(lastIndex) + "%";
		}

		returnString += ": ";

		if (getCurrentPercentInt()>lastIndex) {
			return returnString += "?";
		}

		

		float interpolation = static_cast<float>(getCurrentPercent())-static_cast<float>(firstIndex);

		float interpolatedWinrate 
		= calculateWinrate( firstIndex, lastIndex+1)*(1.0-interpolation)
		+ calculateWinrate( firstIndex+1, lastIndex+1)*interpolation;

		if (interpolatedWinrate==0.0) {
			returnString += "1 in Infinity";
		} else if (interpolatedWinrate>0.5) {
			returnString += std::format("{:.3g}", interpolatedWinrate*100) + "%";
		} else {
			returnString += "1 in " + formatLargeNumbers(static_cast<int>(1.0/interpolatedWinrate));
		}

		return returnString;
	}

	std::string formatLargeNumbers(int number) {
		std::string numberString = std::to_string(number);
		int exponent = numberString.length()-1;
		int exponent3 = exponent/3;
		int significantNumbers = exponent%3+1;
		std::string returnNumberString = numberString.substr(0, significantNumbers);
		// for(int i = 0;i<3-significantNumbers;i++) returnNumberString += " ";
		if (significantNumbers==3) returnNumberString[2] = '0';

		std::string numberSuffix = "";
		switch (exponent3) {
			case 0:
				break;
			case 1:
				numberSuffix = " Thousand";
				break;
			case 2:
				numberSuffix = " Million";
				break;
			case 3:
				numberSuffix = " Billion";
				break;
			case 4:
				numberSuffix = " Trillion";
				break;
			case 5:
				numberSuffix = " Quadrillion";
				break;
			case 6:
				numberSuffix = " Quintillion";
				break;
			case 7:
				numberSuffix = " Sextillion";
				break;
			case 8:
				numberSuffix = " Octillion";
				break;
			case 9:
				numberSuffix = " Nonillion";
				break;
			case 10:
				numberSuffix = " Decillion";
				break;
			case 11:
				numberSuffix = " Undecillion";
				break;
			case 12:
				numberSuffix = " Duodecillion";
				break;
			case 13:
				numberSuffix = " Tredecillion";
				break;
			case 14:
				numberSuffix = " Quattuordecillion";
				break;
			case 15:
				numberSuffix = " Quindecillion";
				break;
			default:
				numberSuffix = "*10^" + std::to_string(exponent3*3);
		}

		return returnNumberString + numberSuffix;
	}

	void updateWinrate(int start, int end) {
        log::info("updateWinrate({},{})",start,end);
		if (start==-1) {
			log::info("SafeZoned!",start,end);
			return;
		}
		for(int i=start;i<end;i++){
			updateWinratePercentage(i);
		}

		
		if (end<100) {
            log::info("index: {} old: {} new: {}",end,m_fields->m_percentageWinrate[end],m_fields->m_percentageWinrate[end]*(1-ALPHA));
			m_fields->m_percentageWinrate[end] = m_fields->m_percentageWinrate[end]*(1-ALPHA);
		}
	}

    void updateWinratePercentage(int index) {
        m_fields->m_percentageDataCount[index]++;
        float localAlpha = 2.0/(m_fields->m_percentageDataCount[index]+1.5);
		// log::info("localAlpha: {}",localAlpha);
        if (localAlpha<ALPHA) {
            localAlpha = ALPHA;
        }
        // log::info("index: {} old: {} new: {}",index,m_fields->m_percentageWinrate[index],localAlpha + m_fields->m_percentageWinrate[index]*(1-localAlpha));
        m_fields->m_percentageWinrate[index] = localAlpha + m_fields->m_percentageWinrate[index]*(1-localAlpha);
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

		std::string timeText = assembleCompletionTimeText();
		m_fields->m_completionTimeLabel->setString(timeText.c_str());

		std::string flatWinText = assembleWinrateText(0,lastIndex,false);
		m_fields->m_winrateLabelFlat->setString(flatWinText.c_str());

		m_fields->m_parentContainer->updateLayout();
	}

	std::string assembleCompletionTimeText() {
		std::string returnString = "Time for ";

		auto lastValidIterator = std::find(m_fields->m_percentageTimeLength.begin(),m_fields->m_percentageTimeLength.end(),-1);
		int lastIndex = lastValidIterator-m_fields->m_percentageTimeLength.begin();

		returnString += fmt::to_string(lastIndex) + "%: ";

		float time = calculateTimeToComplete(lastIndex);

		returnString += formatTime(static_cast<int>(time));

		return returnString;
	}

	std::string formatTime(int time) {
		std::string timeUnit;
		std::string returnNumberString;
		std::string timeString;

		int originalTime = time;

		if (time>60*60*24*7*52) {
			time /= 60*60*24*7*52;
			timeString = formatLargeNumbers(time);
			timeUnit = "year";
		} else if (time>60*60*24) {
			time /= 60*60*24;
			timeString = fmt::to_string(time);
			timeUnit = "day";
		} else if (time>60*60) {
			time /= 60*60;
			timeString = fmt::to_string(time);
			timeUnit = "hour";
		} else if (time>60) {
			time /= 60;
			timeString = fmt::to_string(time);
			timeUnit = "minute";
		} else {
			timeString = fmt::to_string(time);
			timeUnit = "second";
		}
		
		if (time!=1) timeUnit += "s";
		
		return timeString + " " + timeUnit + std::to_string(originalTime);
	}

	void postUpdate(float dt) {

		updateDynamicTextLabels();

		if (m_fields->m_measuringTimeLength){
			measureTimeUpdate(dt);
		}

		// Logic for adding a safe region in the beginning of each attempt 
		// from practice mode or startpos.
		if (m_fields->m_endOfSafeZone == -1 && m_attemptTime>0.8) {
			m_fields->m_endOfSafeZone = getCurrentPercentInt()+1;
		}
		
		PlayLayer::postUpdate(dt);
	}

	void measureTimeUpdate(float dt) {
		if (getCurrentPercent()!=0.0) m_fields->m_currentMeasurement += dt;
			if (m_fields->m_currentIndex!=getCurrentPercentInt()) {
				log::info("measurement: {} {} {}",m_fields->m_currentMeasurement,m_fields->m_currentIndexActive, m_fields->m_percentageTimeLength[m_fields->m_currentIndex]);
				if (m_fields->m_currentIndexActive && m_fields->m_percentageTimeLength[m_fields->m_currentIndex]==-1) {
					m_fields->m_percentageTimeLength[m_fields->m_currentIndex] = m_fields->m_currentMeasurement;

					log::info("Updated");

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