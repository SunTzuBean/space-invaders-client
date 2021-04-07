#include <chrono>
#include "Timeline.h"
#include <iostream>
#include <mutex>



double getCurrentTimeFromChronoAsDouble() {
	auto current_time = std::chrono::system_clock::now();
	auto duration_in_seconds = std::chrono::duration<double>(current_time.time_since_epoch());
	return duration_in_seconds.count();
}

Timeline::Timeline(double tick) {
	this->startTime = getCurrentTimeFromChronoAsDouble();
	this->tick = tick;
	this->paused = false;
	this->lastPausedTime = 0;
	this->elapsedPauseTime = 0;
	this->timeline = NULL;
}
Timeline::Timeline(Timeline* anchor, double tick) : Timeline(tick) {
	this->timeline = anchor;

}


double Timeline::getTime() {
	std::mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);
	if (!paused) {
		double currentTime = getCurrentTimeFromChronoAsDouble();
		double elapsed = currentTime - startTime;
		elapsed -= elapsedPauseTime;
		return elapsed / tick;
	} else {
		double currentTime = lastPausedTime;
		double elapsed = currentTime - startTime;
		elapsed -= elapsedPauseTime;
		//std::cout << "this print statement is necessary " << std::endl;
		// If I don't have this print statement, the pause/unpause does weird things.
		return elapsed / tick;
	}
}


void Timeline::pause() {
	std::mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);
	lastPausedTime = getCurrentTimeFromChronoAsDouble();
	paused = true;
	
}

void Timeline::unpause() {
	std::mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);
	paused = false;
	elapsedPauseTime += getCurrentTimeFromChronoAsDouble() - lastPausedTime;
}

void Timeline::changeTick(double newTick) {
	std::mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);
	tick = newTick;
}

double Timeline::getTick() {
	std::mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);
	return tick;
}