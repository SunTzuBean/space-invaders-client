#pragma once
#include <unordered_map>
#include <vector>
#include <queue>
#include "Event.h"
#include "EventHandler.h"
#include "GameObject.h"

class EventHandler;
class ReplayHandler;

class EventManager {
private:
	struct CompareEvent {
		bool operator()(const Event& e1, const Event& e2) {
			// Highest priority is the lowest timestamp.
			// If e1 happened later than e2 then it's timestamp will be bigger and should go
			// towards the back of the queue.
			return e1.timestamp > e2.timestamp;
		}
	};
	std::unordered_map <int, EventHandler*> handlers;
	std::unordered_map <std::string, std::vector<EventHandler *>> manager;
	std::priority_queue<Event, std::vector<Event>, CompareEvent> queue;


	ReplayHandler *replayHandler;
	std::vector<EventHandler*> container;

	static EventManager* instance;

public:
	/** A boolean to tell if the manager is recording events.*/
	bool isRecording;

	/** A boolean to tell if the manager is replaying events.*/
	bool isReplaying;

	/** A value that tells when the last recording started.*/
	double recordingStartTime;

	/** A value that tells when the last recording ended.*/
	double recordingEndTime;

	/** Useless, old implmentation idea. */
	std::map<int, sf::Vector2f> positionsMap;

	/** Constructs an event manager.*/
	EventManager();

	/** Gets or constructs an instance of an EventManager. 
	* @return an instance of an event manager.
	*/
	static EventManager *getInstance();

	/**
	* Registers an event with its respective handlers that care about it.
	* 
	* @param eventType the type of event that the handler will be associated with.
	* @param handlers the vector of handler pointers that care about the eventType.
	*/
	void reg(std::string eventType, std::vector<EventHandler *>& handlers); // adds to map.
	
	/**
	* Unregisters an event. Unused.
	* 
	* @param eventType the event type that would be unregistered.
	*/
	void unreg(std::string eventType); // removes from map.

	/**
	* Allows an event to be added to a priority queue for handling at a later time. The queue is organized
	* by the timestamp of the event.
	* 
	* @param event the event to add the priority queue.
	*/
	void raise(Event event); // execution strategy for handling, could be immediate, deferred, sent over network, etc.

	void raisePtr(Event* event);

	/**
	* Handles all events in the priority queue.
	*/
	void handleEvents(); // does event handling

	/**
	* Handles all events in priority queue with timestamp less than startRecordingTime + deltaT.
	* 
	* @param deltaT the amount of time that has passed since the last iteration.
	*/
	void handleEvents(double deltaT); // gives criteria for what event to execute next

	/**
	* Function that allows an event handler to be associated with a single event.
	* 
	* @param guid the guid to associated the handler with.
	* @param handler a pointer to the associated handler.
	*/
	void addToHandlers(int guid, EventHandler* handler);

	/**
	* Gets a pointer to a vector of event handlers.
	*/
	std::vector<EventHandler*>* getHandlers(std::string eventType);
};