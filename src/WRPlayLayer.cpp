
#include <Geode/modify/PlayLayer.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include <Geode/utils/random.hpp>
#include "constants.cpp"


using namespace geode::prelude;

class $modify(WRPlayLayer, PlayLayer){
	struct Fields {
		std::array<float,100> m_percentageWinrate;
        std::array<int,100> m_percentageDataCount;
		std::array<float,100> m_percentageTimeLength;
		std::set<std::string> m_linkedLevels; 
		std::string m_levelId;
		std::string m_levelName;
		int m_attemptsStart;
		float m_alpha; 
		double m_currentWinrate;
		double m_currentWinrateStartpos = -1;
		double m_currentTime;
		bool m_measuringTimeLength;
		float m_currentMeasurement;
		int m_currentIndex;
		bool m_currentIndexActive;
		float m_startingPercentage = -1;
		int m_endOfSafeZone = -1;
		double m_safeZoneDuration = 1.2;
		CCNodeRGBA* m_parentContainer = nullptr;
		CCLabelBMFont* m_winrateLabel = nullptr;
		CCLabelBMFont* m_rarityLabel = nullptr;
		CCLabelBMFont* m_winrateLabelFlat = nullptr;
		std::string m_winrateLabelFlatString;
		CCLabelBMFont* m_completionTimeLabel = nullptr;
		std::string m_completionTimeLabelString;

		~Fields() {
			for (std::string levelId : m_linkedLevels) {
				Mod::get()->setSavedValue(
				levelId + "-winrate", m_percentageWinrate);

				Mod::get()->setSavedValue(
				levelId + "-datacount", m_percentageDataCount);

				Mod::get()->setSavedValue(
				levelId + "-timelength", m_percentageTimeLength);

				Mod::get()->setSavedValue(
				levelId + "-linked", m_linkedLevels);
			}	
			if (PlayLayer::get()) {
				auto pl = static_cast<WRPlayLayer*>(PlayLayer::get());
				Mod::get()->setSavedValue(
				m_levelId + "-attempts", m_attemptsStart+pl->m_attempts);
			}
        }
	};

	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        // log::info("init()");

		m_fields->m_alpha = Mod::get()->getSettingValue<float>("settings-alpha");
		m_fields->m_safeZoneDuration = Mod::get()->getSettingValue<double>("settings-safezone");

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

		m_fields->m_percentageWinrate 
		= Mod::get()->getSavedValue<std::array<float,100>>(
		levelId + "-winrate", m_fields->m_percentageWinrate);

		m_fields->m_percentageDataCount 
		= Mod::get()->getSavedValue<std::array<int,100>>(
		levelId+ "-datacount", m_fields->m_percentageDataCount);

		m_fields->m_percentageTimeLength 
		= Mod::get()->getSavedValue<std::array<float,100>>(
		levelId + "-timelength", m_fields->m_percentageTimeLength);

		m_fields->m_linkedLevels 
		= Mod::get()->getSavedValue<std::set<std::string>>(
		levelId + "-linked", m_fields->m_linkedLevels);

		m_fields->m_linkedLevels.insert(levelId);
		m_fields->m_levelId = levelId;
		m_fields->m_attemptsStart = level->m_attempts;


		// Saving the id in a list here we can loop through later
		auto dataAll 
		= Mod::get()->getSavedValue<std::set<std::string>>("all-level-ids");
		dataAll.insert(levelId);
		Mod::get()->setSavedValue("all-level-ids", dataAll);

		m_fields->m_levelName = level->m_levelName;

		Mod::get()->setSavedValue(levelId + "-levelname",m_fields->m_levelName);

		for (std::string id : dataAll) {
			log::info("{} 1 {}",id, 
			Mod::get()->getSavedValue<std::string>(id + "-levelname", ""));
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


		std::string corner 
		= Mod::get()->getSettingValue<std::string>("label-corner");

		
		auto uiLayer = this->getChildByID("UILayer");
		
		if (corner == "Bottom Left") { // Done
			m_fields->m_parentContainer->setLayout(
				ColumnLayout::create()
					->setGap(10.f)->setGrowCrossAxis(true)
					->setAxisAlignment(AxisAlignment::Start)
					->setCrossAxisLineAlignment(AxisAlignment::Start));

			m_fields->m_parentContainer->setPosition(
			uiLayer->getPosition() + CCPoint { 10.0f, 10.0f });
		} else if (corner == "Bottom Right") {
			m_fields->m_parentContainer->setLayout(
				ColumnLayout::create()
					->setGap(10.f)->setGrowCrossAxis(true)
					->setAxisAlignment(AxisAlignment::Start)
					->setCrossAxisLineAlignment(AxisAlignment::End));

			m_fields->m_parentContainer->setAnchorPoint(CCPoint{1.0f,0.0f});
			m_fields->m_parentContainer->setPosition(
			uiLayer->getPosition() 
			+ CCPoint { uiLayer->getContentWidth() - 10.0f,10.0f });
		} else if (corner == "Top Right") {
			m_fields->m_parentContainer->setLayout(
				ColumnLayout::create()
					->setGap(10.f)->setGrowCrossAxis(true)
					->setAxisAlignment(AxisAlignment::End)
					->setCrossAxisLineAlignment(AxisAlignment::End));

			m_fields->m_parentContainer->setAnchorPoint(CCPoint{1.0f,1.0f});
			m_fields->m_parentContainer->setPosition(
			uiLayer->getPosition() 
			+ CCPoint { uiLayer->getContentWidth()-10.0f 
			, uiLayer->getContentHeight()-10.0f });
		} else {
			m_fields->m_parentContainer->setLayout(
				ColumnLayout::create()
					->setGap(10.f)->setGrowCrossAxis(true)
					->setAxisAlignment(AxisAlignment::End)
					->setCrossAxisLineAlignment(AxisAlignment::Start));

			m_fields->m_parentContainer->setAnchorPoint(CCPoint{0.0f,1.0f});
			m_fields->m_parentContainer->setPosition(
			uiLayer->getPosition() 
			+ CCPoint { 10.0f, uiLayer->getContentHeight()-10.0f });
		}

		float labelSize = 
		0.35 * Mod::get()->getSettingValue<float>("label-size");

		m_fields->m_parentContainer->setScale(labelSize);
		m_fields->m_parentContainer->setContentHeight(700);
		m_fields->m_parentContainer->setContentWidth(700);

		uiLayer->addChild(m_fields->m_parentContainer);

		m_fields->m_parentContainer->setID("parent-label"_spr);


		if (!level->isPlatformer() 
		|| Mod::get()->getSettingValue<bool>("settings-platformer")) {
			if (Mod::get()->getSettingValue<bool>("winrate-dynamic")) {
				m_fields->m_winrateLabel 
				= CCLabelBMFont::create("Winrate Now is TEST"
				, "bigFont.fnt");

				m_fields->m_parentContainer
				->addChild(m_fields->m_winrateLabel);

				m_fields->m_winrateLabel->setAnchorPoint({0,0});
				
				m_fields->m_parentContainer->updateLayout();
			}
			if (Mod::get()->getSettingValue<bool>("winrate-flat")) {
				m_fields->m_winrateLabelFlat 
				= CCLabelBMFont::create("Winrate is TEST"
				, "bigFont.fnt");

				m_fields->m_parentContainer
				->addChild(m_fields->m_winrateLabelFlat);

				m_fields->m_winrateLabelFlat->setAnchorPoint({0,0});
				
				m_fields->m_parentContainer->updateLayout();
			}
			if (Mod::get()->getSettingValue<bool>("completion-time")) {
				m_fields->m_completionTimeLabel 
				= CCLabelBMFont::create("Time from 0 to 100 is TEST"
				, "bigFont.fnt");

				m_fields->m_parentContainer
				->addChild(m_fields->m_completionTimeLabel);

				m_fields->m_completionTimeLabel->setAnchorPoint({0,0});
			}
			if (Mod::get()->getSettingValue<bool>("rarity-label")) {
				m_fields->m_rarityLabel 
				= CCLabelBMFont::create("Run Rarity is TEST"
				, "bigFont.fnt");

				m_fields->m_parentContainer
				->addChild(m_fields->m_rarityLabel);

				m_fields->m_rarityLabel->setAnchorPoint({0,0});
				
				m_fields->m_parentContainer->updateLayout();
			}

			m_fields->m_parentContainer
			->setOpacity(static_cast<int>(
			255.0*Mod::get()->getSettingValue<float>("label-opacity")));

			m_fields->m_parentContainer->updateLayout();
		}

		auto lastElementWithZeroData 
		= std::find(m_fields->m_percentageDataCount.begin()
		,m_fields->m_percentageDataCount.end(),0);

		int lastIndexWinrate 
		= lastElementWithZeroData-m_fields->m_percentageDataCount.begin()+1;

		m_fields->m_currentWinrate = calculateWinrate(0, lastIndexWinrate);

		auto lastValidIterator 
		= std::find(m_fields->m_percentageTimeLength.begin(),
		m_fields->m_percentageTimeLength.end(),-1);

		int lastIndexTime 
		= lastValidIterator-m_fields->m_percentageTimeLength.begin();

		m_fields->m_currentTime = calculateTimeToComplete(lastIndexTime); 


		if (m_fields->m_parentContainer) { 
			updateStaticTextLabels();	
			m_fields->m_currentWinrateStartpos = -1;
			m_fields->m_currentMeasurement = 0;
			m_fields->m_currentIndex = getCurrentPercentInt();
			m_fields->m_currentIndexActive 
			= (getCurrentPercent()==0.0) ? true : false;

			m_fields->m_endOfSafeZone = -1;
			m_fields->m_startingPercentage = getCurrentPercentInt();
			log::info("reset {}",getCurrentPercentInt());
		}

		return true;
	}

	void levelComplete() {
		int startPercentage 
		= (m_fields->m_startingPercentage==0.0) ? 0 : m_fields->m_endOfSafeZone;
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
			m_fields->m_currentWinrateStartpos = -1;
			m_fields->m_currentMeasurement = 0;
			m_fields->m_currentIndex = getCurrentPercentInt();
			m_fields->m_currentIndexActive 
			= (getCurrentPercent()==0.0) ? true : false;

			m_fields->m_endOfSafeZone = -1;
			m_fields->m_startingPercentage = getCurrentPercentInt();

			log::info("reset {}",getCurrentPercentInt());
		}
	}

	void postUpdate(float dt) {

		updateDynamicTextLabels();

		if (m_fields->m_measuringTimeLength){
			measureTimeUpdate(dt);
		}

		// Logic for adding a safe region in the beginning of each attempt 
		// from practice mode or startpos.
		if (m_fields->m_endOfSafeZone == -1 
		&& m_attemptTime>m_fields->m_safeZoneDuration) {
			m_fields->m_endOfSafeZone = getCurrentPercentInt()+1;
			log::info("{}",m_fields->m_endOfSafeZone);
		}

		PlayLayer::postUpdate(dt);
	}

	// Stores the winrate at the start of the attempt, for use on run rarity
	void updateStartposWinrate() {
		auto lastElementWithZeroData 
		= std::find(m_fields->m_percentageDataCount.begin(),
		m_fields->m_percentageDataCount.end(),0);
		int lastIndex 
		= lastElementWithZeroData-m_fields->m_percentageDataCount.begin();

		double interpolation 
		= (static_cast<double>(getCurrentPercent()) 
		- static_cast<double>(getCurrentPercentInt()));

		double interpolatedWinrate 
		= calculateWinrate( 
		getCurrentPercentInt(), lastIndex+1)*(1.0-interpolation)
		+ calculateWinrate( getCurrentPercentInt()+1, lastIndex+1)
		*interpolation;

		m_fields->m_currentWinrateStartpos = interpolatedWinrate;
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
			expectedTime = (static_cast<double>(
			m_fields->m_percentageTimeLength[i])+expectedTime)
			/std::max({static_cast<double>(m_fields->m_percentageWinrate[i])
			,LOWERLIMIT});
		}

        return expectedTime;
    }

	std::string formatLargeNumbers(double number) {

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

	void updateWinrate(int start, int end) {
		
        log::info("updateWinrate({},{})",start,end);
		if (start==-1 || (end<start)) {
			// log::info("SafeZoned!");
			return;
		}
		for(int i=start;i<end;i++){
			updateWinratePercentage(i,true,false);
		}

		
		if (end<100) {
			updateWinratePercentage(end, false,false);
		}
	}

    void updateWinratePercentage(int index, bool passed, bool temp) {
		// Initial higher alpha
        m_fields->m_percentageDataCount[index]++;
		
		if (passed) {
			m_fields->m_percentageWinrate[index] 
			= m_fields->m_alpha 
			+ m_fields->m_percentageWinrate[index]*(1.0f-m_fields->m_alpha);
		} else {
			m_fields->m_percentageWinrate[index] 
			= m_fields->m_percentageWinrate[index]*(1.0f-m_fields->m_alpha);
		}   
    }

	void updateDynamicTextLabels() {
		auto lastElementWithZeroData 
		= std::find(m_fields->m_percentageDataCount.begin(),
		m_fields->m_percentageDataCount.end(),0);
		int lastIndex 
		= lastElementWithZeroData-m_fields->m_percentageDataCount.begin();
		if (m_fields->m_winrateLabel) {
			m_fields->m_winrateLabel->setString(
			(assembleWinrateText(getCurrentPercentInt()
			,lastIndex,true)).c_str());
			m_fields->m_parentContainer->updateLayout();
		}

		if (m_fields->m_rarityLabel) {
			if (m_fields->m_currentWinrateStartpos == -1) {
				updateStartposWinrate();
			}
			m_fields->m_rarityLabel->setString(
			(assembleRarityText(lastIndex)).c_str());

			m_fields->m_parentContainer->updateLayout();
		}
	}

	std::string assembleWinrateText(int firstIndex, int lastIndex, bool dynamic){
		std::string returnString = "";
		returnString += "Winrate";

		if (dynamic) returnString += " now" ;

		if (lastIndex<100) {
			returnString += " to " 
			+ fmt::to_string(std::max({0,lastIndex-1})) + "%";
		}

		returnString += ": ";

		if (getCurrentPercentInt()==100) {
			return returnString + "100%";
		}

		if (getCurrentPercentInt()>=lastIndex) {
			return returnString + "?";
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
			returnString += fmt::format("{:.3g}", interpolatedWinrate*100) + "%";
		} else {
			returnString += "1 in " 
			+ formatLargeNumbers((1.0/interpolatedWinrate));
		}

		return returnString;
	}

	std::string assembleCompletionTimeText() {
		std::string returnString = "Time";

		auto lastValidIterator 
		= std::find(m_fields->m_percentageTimeLength.begin()
		,m_fields->m_percentageTimeLength.end(),-1);

		int lastIndex = lastValidIterator-m_fields->m_percentageTimeLength.begin();

		// We never die at 100% but when we have died at 99%, it will say to 100
		returnString += " for " 
		+ fmt::to_string(std::max({0,lastIndex-(lastIndex!=100)})) 
		+ "%";

		returnString += ": ";

		double time = calculateTimeToComplete(lastIndex);

		returnString += (time != 1.0) ? formatTime(time) : "?";

		return returnString;
	}

	std::string assembleRarityText(int lastIndex) {
		std::string returnString = "";
		returnString += "Run Rarity: ";

		

		if (getCurrentPercentInt()>lastIndex) {
			return returnString + "?";
		}

		double interpolation 
		= (static_cast<double>(getCurrentPercent()) 
		- static_cast<double>(getCurrentPercentInt()));

		double interpolatedWinrate 
		= calculateWinrate( getCurrentPercentInt(), lastIndex+1)*(1.0-interpolation)
		+ calculateWinrate( getCurrentPercentInt()+1, lastIndex+1)*interpolation;

		if (interpolatedWinrate==0) return returnString += "?";

		double rarityWinrate 
		= m_fields->m_currentWinrateStartpos/interpolatedWinrate;

		if (rarityWinrate==0.0) {
			return returnString += "1 in Infinity";
		}  else {
			returnString += "1 in " + formatLargeNumbers((1.0/rarityWinrate));
		}

		return returnString;
	}

	void updateStaticTextLabels() {
		auto lastElementWithZeroData 
		= std::find(m_fields->m_percentageDataCount.begin()
		,m_fields->m_percentageDataCount.end(),0);

		int lastIndex 
		= lastElementWithZeroData-m_fields->m_percentageDataCount.begin();

		if (m_fields->m_completionTimeLabel){
			m_fields->m_completionTimeLabelString = assembleCompletionTimeText();
			m_fields->m_completionTimeLabel
			->setString(m_fields->m_completionTimeLabelString.c_str());
		}

		if (m_fields->m_winrateLabelFlat){
			m_fields->m_winrateLabelFlatString = assembleWinrateText(0,lastIndex,false);
			m_fields->m_winrateLabelFlat->setString(m_fields->m_winrateLabelFlatString.c_str());
		}

		m_fields->m_parentContainer->updateLayout();
	}

	void updateChange() {
		// Calculating the new winrate
		auto lastElementWithZeroData 
		= std::find(m_fields->m_percentageDataCount.begin()
		,m_fields->m_percentageDataCount.end(),0);

		int lastIndexWinrate 
		= lastElementWithZeroData-m_fields->m_percentageDataCount.begin()+1;

		double newWinrate = calculateWinrate(0, lastIndexWinrate);

		// Calculating the new completion time
		auto lastValidIterator 
		= std::find(m_fields->m_percentageTimeLength.begin()
		,m_fields->m_percentageTimeLength.end(),-1);

		int lastIndexTime 
		= lastValidIterator-m_fields->m_percentageTimeLength.begin();

		double newTime = calculateTimeToComplete(lastIndexTime);

		double differenceTime = newTime-m_fields->m_currentTime;
		double differenceWinrate = newWinrate-m_fields->m_currentWinrate;

		if (m_fields->m_completionTimeLabel) {
			std::string changesTime = m_fields->m_completionTimeLabelString;
			
			changesTime += ((differenceWinrate<0) ? " (+" : " (-") 
			+ formatTime(std::abs(differenceTime)) + ")";

			m_fields->m_completionTimeLabel->setString(changesTime.c_str());
			
			m_fields->m_currentTime = newTime;
		}

		if (m_fields->m_winrateLabelFlat) {
			std::string changesWinrate = m_fields->m_winrateLabelFlatString;

			if (m_fields->m_currentWinrate>0.1) {
				changesWinrate += ((differenceWinrate>=0) ? " (+" : " (-") 
				+ formatLargeNumbers(std::abs(differenceWinrate*100)) + "%)";
			} else {
				double differenceWinrateInverse 
				= 1/newWinrate-1/m_fields->m_currentWinrate;

				changesWinrate += ((differenceWinrate<0) ? " (+" : " (-") 
				+ formatLargeNumbers(std::abs(differenceWinrateInverse)) + ")";
			}

			m_fields->m_winrateLabelFlat->setString(changesWinrate.c_str());

			m_fields->m_currentWinrate = newWinrate;
		}
		m_fields->m_parentContainer->updateLayout();
	}	

	

	std::string formatTime(double time) {
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
				timeString = formatLargeNumbers(time);
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

	void measureTimeUpdate(float dt) {
		if (getCurrentPercent()!=0.0) m_fields->m_currentMeasurement += dt;
			if (m_fields->m_currentIndex!=getCurrentPercentInt()) {
				if (m_fields->m_currentIndexActive 
					&& m_fields->m_percentageTimeLength[m_fields->m_currentIndex]==-1 
					&& getCurrentPercentInt()==m_fields->m_currentIndex+1) {

					m_fields->m_percentageTimeLength[m_fields->m_currentIndex] 
					= m_fields->m_currentMeasurement;

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