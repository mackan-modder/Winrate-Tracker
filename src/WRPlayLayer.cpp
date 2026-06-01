
#include <Geode/modify/PlayLayer.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>

const float ALPHA = 0.25;

using namespace geode::prelude;

class $modify(WRPlayLayer, PlayLayer){
	struct Fields {
		std::array<float,100> m_percentageWinrate;
        std::array<int,100> m_percentageDataCount;
		float m_startingPercentage;
		std::string m_levelStringWinrate;
		std::string m_levelStringDataCount;
		CCNodeRGBA* m_parentLabel = nullptr;
		CCLabelBMFont* m_winrate_label = nullptr;

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


		m_fields->m_parentLabel = CCNodeRGBA::create();
		m_fields->m_parentLabel->setCascadeColorEnabled(true);
		m_fields->m_parentLabel->setCascadeOpacityEnabled(true);

		m_fields->m_parentLabel->setLayout(RowLayout::create());

		auto uiLayer = this->getChildByID("UILayer");

		m_fields->m_parentLabel->setPosition(uiLayer->getPosition() + CCPoint { 55.f, 20.f });
		m_fields->m_parentLabel->setScale(0.35);

		uiLayer->addChild(m_fields->m_parentLabel);

		m_fields->m_parentLabel->setID("parent-label"_spr);

		m_fields->m_winrate_label = CCLabelBMFont::create("Winrate is TEST", "bigFont.fnt");

		m_fields->m_parentLabel->addChild(m_fields->m_winrate_label);

		return true;
	}

	void levelComplete() {
		updateWinrate(m_fields->m_startingPercentage,100);
		
		PlayLayer::levelComplete();
	}
	
	void resetLevel() {	
        
        log::info("resetLevel()");

		PlayLayer::resetLevel();
		
        log::info("Total winrate is 1 in {}",calculateWinrate(0,100));

		m_fields->m_startingPercentage = getCurrentPercentInt();
	}

    std::string calculateWinrate(int start, int end) {
        double product = 1;
		std::string returnString;

        for(int i=start;i<end;i++){
            product *= static_cast<double>(m_fields->m_percentageWinrate[i]);
			log::info("Product is {} from {} which was {}",product,static_cast<double>(m_fields->m_percentageWinrate[i]),m_fields->m_percentageWinrate[i]);
        }

		if (product==0.0) {
			returnString = "Infinity";
		} else {
			returnString = fmt::to_string(1.0/product);
		}

        return returnString;
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
};