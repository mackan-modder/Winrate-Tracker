
#include <Geode/modify/PlayLayer.hpp>

const float ALPHA = 0.1;

using namespace geode::prelude;

class $modify(WRPlayLayer, PlayLayer){
	struct Fields {
		float percentageWinrate[100];
		float startingPercentage;
	};

	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
		if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        geode::log::info("init()");

		for(int i=0;i<100;i++){
			m_fields->percentageWinrate[i] = 1;
		}

		return true;
	}

	void levelComplete() {
		updateWinrate(m_fields->startingPercentage,100);
		
		PlayLayer::levelComplete();
	}
	
	void resetLevel() {	
		PlayLayer::resetLevel();

		m_fields->startingPercentage = getCurrentPercentInt();
	}

	void updateWinrate(int start, int end) {
        geode::log::info("updateWinrate()");
		for(int i=start;i<end;i++){
            geode::log::info("index: {} old: {} new: {}",i,m_fields->percentageWinrate[i],ALPHA + m_fields->percentageWinrate[i]*(1-ALPHA));
			m_fields->percentageWinrate[i] = ALPHA + m_fields->percentageWinrate[i]*(1-ALPHA);
		}

		
		if (end<100) {
            geode::log::info("index: {} old: {} new: {}",end,m_fields->percentageWinrate[end],m_fields->percentageWinrate[end]*(1-ALPHA));
			m_fields->percentageWinrate[end] = m_fields->percentageWinrate[end]*(1-ALPHA);
		}
	}
};