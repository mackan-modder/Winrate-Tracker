
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

		m_fields->m_completionTimeLabel = CCLabelBMFont::create("Time from 0 to 100 is TEST", "bigFont.fnt");

		m_fields->m_parentContainer->addChild(m_fields->m_completionTimeLabel);

		m_fields->m_completionTimeLabel->setAnchorPoint({0,0});

		m_fields->m_parentContainer->updateLayout();
		return true;
	}

	void levelComplete() {
		int startPercentageWithKindness = (m_fields->m_startingPercentage==0.0) ? 0 : static_cast<int>(m_fields->m_startingPercentage+1.5);
		updateWinrate(startPercentageWithKindness,100);
		
		PlayLayer::levelComplete();
	}
	
	void resetLevel() {	
        
        log::info("resetLevel()");

		PlayLayer::resetLevel();
		
        log::info("Total winrate is 1 in {}",calculateWinrate(0,100));

		m_fields->m_startingPercentage = getCurrentPercent();
	}

    double calculateWinrate(int start, int end) {
        double product = 1;

        for(int i=start;i<std::min({end,100});i++){
            product *= static_cast<double>(m_fields->m_percentageWinrate[i]);
        }

        return product;
    }

	std::string assembleWinrateText(int lastIndex){
		std::string returnString = "Winrate";

		if (lastIndex<99) {
			returnString += " to " + std::to_string(lastIndex) + "%";
		}

		returnString += ": ";

		if (getCurrentPercentInt()>lastIndex) {
			returnString += "?";
		}

		int firstIndex = static_cast<int>(getCurrentPercent());

		double interpolation = static_cast<double>(getCurrentPercent())-static_cast<double>(firstIndex);

		double interpolatedWinrate 
		= calculateWinrate( firstIndex, lastIndex+1)*(1.0-interpolation)
		+ calculateWinrate( firstIndex+1, lastIndex+1)*interpolation;

		if (interpolatedWinrate==0.0) {
			returnString += "1 in Infinity";
		} else if (interpolatedWinrate>0.5) {
			returnString += std::format("{:.3g}", interpolatedWinrate*100) + "%";
		} else {
			returnString += "1 in " + formatLargeNumbers(1.0/interpolatedWinrate);
		}

		return returnString;
	}

	std::string formatLargeNumbers(double number) {
		std::string numberFormated = "";
		std::string numberSuffix = "";

		int suffixIndex = 0;
		if (number<100.0) {
			return std::format("{:.2g}", number);
		}

		// Redo this. I believe remakinbg it to a string and then chosing the first 3 letters is the way.
		while (number*0.001!=0) {
			number *= 0.001;
			suffixIndex += 1;
		}

		return std::format("{:.3g}", number) + " " + numberSuffix;
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
		log::info("localAlpha: {}",localAlpha);
        if (localAlpha<ALPHA) {
            localAlpha = ALPHA;
        }
        log::info("index: {} old: {} new: {}",index,m_fields->m_percentageWinrate[index],localAlpha + m_fields->m_percentageWinrate[index]*(1-localAlpha));
        m_fields->m_percentageWinrate[index] = localAlpha + m_fields->m_percentageWinrate[index]*(1-localAlpha);
    }

	void updateTextLabels() {
		auto lastElementWithZeroData = std::find(m_fields->m_percentageDataCount.begin(),m_fields->m_percentageDataCount.end(),0);
		int lastIndex = lastElementWithZeroData-m_fields->m_percentageDataCount.begin();
		m_fields->m_winrateLabel->setString(assembleWinrateText(lastIndex).c_str());
		std::string timeText = "Completion time: " + fmt::to_string(static_cast<double>(getCurrentPercent())-static_cast<double>(static_cast<int>(getCurrentPercent())));
		m_fields->m_completionTimeLabel->setString(timeText.c_str());
		m_fields->m_parentContainer->updateLayout();

	}

	void postUpdate(float dt) {

		updateTextLabels();
		
		PlayLayer::postUpdate(dt);
	}
};