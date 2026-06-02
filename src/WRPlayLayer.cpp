
#include <Geode/modify/PlayLayer.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include <Geode/utils/random.hpp>

const float ALPHA = 0.25;

using namespace geode::prelude;

class $modify(WRPlayLayer, PlayLayer){
	struct Fields {
		std::array<float,100> m_percentageWinrate;
        std::array<int,100> m_percentageDataCount;
		float m_startingPercentage;
		std::string m_levelStringWinrate;
		std::string m_levelStringDataCount;
		CCNodeRGBA* m_parentContainer = nullptr;
		CCLabelBMFont* m_winrateLabel = nullptr;
		CCLabelBMFont* m_winrateLabelFlat = nullptr;
		CCLabelBMFont* m_completionTimeLabel = nullptr;

		~Fields() {
			Mod::get()->setSavedValue(m_levelStringWinrate, m_percentageWinrate);
			Mod::get()->setSavedValue(m_levelStringDataCount, m_percentageDataCount);
        }
	};

	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        log::info("init()");

		std::array<float,100> tempPercentageWinrate;
		std::array<int,100> tempmPercentageDataCount;
		for(int i=0;i<100;i++){	
			tempPercentageWinrate[i] = 1.0;
			tempmPercentageDataCount[i] = 0;
		}

		int ourLevelId = level->m_levelID.value();
		if (!ourLevelId) {
			ourLevelId = EditorIDs::getID(level);
		}

		m_fields->m_levelStringWinrate = fmt::to_string(ourLevelId).append("-winrate");

		m_fields->m_levelStringDataCount = fmt::to_string(ourLevelId).append("-datacount");

		auto data1 = Mod::get()->getSavedValue<std::array<float,100>>(m_fields->m_levelStringWinrate, tempPercentageWinrate);
		auto data2 = Mod::get()->getSavedValue<std::array<int,100>>(m_fields->m_levelStringDataCount, tempmPercentageDataCount);

		memcpy(&m_fields->m_percentageWinrate, &data1, 100*sizeof(float));
		memcpy(&m_fields->m_percentageDataCount, &data2, 100*sizeof(int));
		
		log::info("Total winrate is 1 in {}",calculateWinrate(0,100));


		m_fields->m_parentContainer = CCNodeRGBA::create();
		m_fields->m_parentContainer->setCascadeColorEnabled(true);
		m_fields->m_parentContainer->setCascadeOpacityEnabled(true);

		m_fields->m_parentContainer->setLayout(
			RowLayout::create()
				->setGap(10.f)->setGrowCrossAxis(true)
				->setAxisAlignment(AxisAlignment::Start));

		auto uiLayer = this->getChildByID("UILayer");

		m_fields->m_parentContainer->setPosition(uiLayer->getPosition() + CCPoint { 10.0f, 10.0f });
		m_fields->m_parentContainer->setScale(0.35);
		m_fields->m_parentContainer->setOpacity(100);
		m_fields->m_parentContainer->setContentHeight(500);
		m_fields->m_parentContainer->setContentWidth(500);

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

		m_fields->m_parentContainer->updateLayout();

		updateStaticTextLabels();

		return true;
	}

	void levelComplete() {
		int startPercentageWithKindness = (m_fields->m_startingPercentage==0.0) ? 0 : static_cast<int>(m_fields->m_startingPercentage+1.5);
		updateWinrate(startPercentageWithKindness,100);
		updateStaticTextLabels();

		PlayLayer::levelComplete();
	}
	
	void resetLevel() {	

		PlayLayer::resetLevel();

		

		m_fields->m_startingPercentage = getCurrentPercent();
	}

    float calculateWinrate(int start, int end) {
        float product = 1;

        for(int i=start;i<std::min({end,100});i++){
            product *= static_cast<float>(m_fields->m_percentageWinrate[i]);
        }

        return product;
    }

	std::string assembleWinrateText(int firstIndex, int lastIndex, bool dynamic){
		std::string returnString = "";
		if (dynamic) returnString += "Current ";
		returnString += "Winrate";

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
		} else if ((interpolatedWinrate>=0.01)){
			returnString += "1 in " + std::format("{:.2g}", 1/interpolatedWinrate);
		} else {
			returnString += "1 in " + formatLargeNumbers(1.0/interpolatedWinrate);
		}

		return returnString;
	}

	std::string formatLargeNumbers(float number) {
		
		if (number<100.0) {
			return std::format("{:.2g}", number);
		}

		

		int intNumber = static_cast<int>(number);
		std::string numberString = std::to_string(intNumber);
		int exponent = numberString.length();
		int exponent3 = exponent/3;
		int significantNumbers = exponent%3+1;
		std::string returnNumberString = numberString.substr(0, significantNumbers);
		// for(int i = 0;i<3-significantNumbers;i++) returnNumberString += " ";

		std::string numberSuffix = "";
		switch (exponent3) {
			case 0:
				break;
			case 1:
				numberSuffix = "\tThousand";
				break;
			case 2:
				numberSuffix = "\tMillion";
				break;
			case 3:
				numberSuffix = "\tBillion";
				break;
			case 4:
				numberSuffix = "\tTrillion";
				break;
			case 5:
				numberSuffix = "\tQuadrillion";
				break;
			case 6:
				numberSuffix = "\tQuintillion";
				break;
			case 7:
				numberSuffix = "\tSextillion";
				break;
			case 8:
				numberSuffix = "\tOctillion";
				break;
			case 9:
				numberSuffix = "\tNonillion";
				break;
			case 10:
				numberSuffix = "\tDecillion";
				break;
			case 11:
				numberSuffix = "\tUndecillion";
				break;
			case 12:
				numberSuffix = "\tDuodecillion";
				break;
			case 13:
				numberSuffix = "\tTredecillion";
				break;
			case 14:
				numberSuffix = "\tQuattuordecillion";
				break;
			case 15:
				numberSuffix = "\tQuindecillion";
				break;
			default:
				numberSuffix = "\t*10^" + std::to_string(exponent3*3);
		}

		return returnNumberString + numberSuffix;
	}

	void updateWinrate(int start, int end) {
        log::info("updateWinrate({},{})",start,end);
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

		std::string timeText = "Completion time: " + fmt::to_string(255);
		m_fields->m_completionTimeLabel->setString(timeText.c_str());

		m_fields->m_winrateLabelFlat->setString(assembleWinrateText(0,lastIndex,false).c_str());

		m_fields->m_parentContainer->updateLayout();
	}

	void postUpdate(float dt) {

		updateDynamicTextLabels();
		
		PlayLayer::postUpdate(dt);
	}
};