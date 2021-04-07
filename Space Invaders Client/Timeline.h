#pragma once
#include <chrono>
#include <mutex>

class Timeline {
private:
	
	double startTime;
	double tick;
	//std::mutex mutex;
	bool paused;
	double lastPausedTime;
	double elapsedPauseTime;
	Timeline* timeline;
	

public:
	

	Timeline(double tick);
	Timeline(Timeline* anchor, double tick);

	
	/**
	* returnst the elapsed time in ms from the anchor time.
	* @return the elapsed time in MS from the start time that this Timeline was given.
	*/
	double getTime();
	void pause();
	void unpause();
	void changeTick(double newTick);
	double getTick();

};