
#include <Geode/modify/PlayLayer.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>

const float ALPHA = 0.25;

using namespace geode::prelude;

class $modify(WRPlayLayer, PlayLayer){
	struct Fields {
		std::array<float,100> percentageWinrate;
        std::array<int,100> percentageDataCount;
		float startingPercentage;

		~Fields() {
			Mod::get()->setSavedValue("data1", percentageWinrate);
			Mod::get()->setSavedValue("data2", percentageDataCount);
        }
	};

	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        geode::log::info("init()");

		std::array<float,100> tempPercentageWinrate;
		std::array<int,100> tempPercentageDataCount;
		for(int i=0;i<100;i++){	
			tempPercentageWinrate[i] = 1;
			tempPercentageDataCount[i] = 0;
		}

		geode::SeedValueRSV ourLevelId = level->m_levelID;

		if (!ourLevelId) {
			geode::log::info("We are in a editor level");
		}

        geode::log::info("CURRENT ID {}",level->m_levelID);

		auto data1 = Mod::get()->getSavedValue<std::array<float,100>>("data1", tempPercentageWinrate);
		auto data2 = Mod::get()->getSavedValue<std::array<int,100>>("data2", tempPercentageDataCount);

		memcpy(&m_fields->percentageWinrate, &data1, 100);
		memcpy(&m_fields->percentageDataCount, &data2, 100);
		

		return true;
	}

	void levelComplete() {
		updateWinrate(m_fields->startingPercentage,100);
		
		PlayLayer::levelComplete();
	}
	
	void resetLevel() {	
        
        geode::log::info("resetLevel()");

		PlayLayer::resetLevel();
		
        

		m_fields->startingPercentage = getCurrentPercentInt();
	}

    float calculateWinrate(int start, int end) {
        float product = 1;
        for(int i=start;i<end;i++){
            product *= m_fields->percentageWinrate[i];
        }
        return product;
    }

	void updateWinrate(int start, int end) {
        geode::log::info("updateWinrate()");
		for(int i=start;i<end;i++){
            geode::log::info("index: {} old: {} new: {}",i,m_fields->percentageWinrate[i],ALPHA + m_fields->percentageWinrate[i]*(1-ALPHA));
			
		}

		
		if (end<100) {
            geode::log::info("index: {} old: {} new: {}",end,m_fields->percentageWinrate[end],m_fields->percentageWinrate[end]*(1-ALPHA));
			m_fields->percentageWinrate[end] = m_fields->percentageWinrate[end]*(1-ALPHA);
		}
	}

    void updateWinratePercentage(int index) {
        m_fields->percentageDataCount[index]++;
        float localAlpha = 1.0/(m_fields->percentageDataCount[index]+1);
        if (localAlpha<ALPHA) {
            localAlpha = ALPHA;
        }
        m_fields->percentageWinrate[index] = ALPHA + m_fields->percentageWinrate[index]*(1-ALPHA);
    }
};