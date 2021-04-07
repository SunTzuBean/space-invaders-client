#pragma once
#include "Event.h"
#include "GameObject.h"
#include "Timeline.h"
#include "ScriptManager.h"
#include "dukglue/dukglue.h"
#include "Bullet.h"
#include "Enemy.h"

#include <zmq.hpp>
#include <iostream>
#include <fstream>

class GameObject;
class Player;
class Platform;
class Spawn;
class Boundary;
class Timeline;
class ScriptManager;

#define COLLISION_PLAT 0
#define COLLISION_VIEW 1
#define COLLISION_DEAD 2

#define ADD_POINTS "addPoints"
#define PLAYER_DEAD "playerDead"
#define WINNER "winner"

class EventHandler {
public:
	/**
	* Each event handler must be able to handle an event.
	* 
	* @param event the event to handle.
	*/
	virtual void onEvent(Event& event) = 0;
};

/**
* Handler for platforms.
*/
class BulletHandler : public EventHandler {
public:
	GameObject* obj;
	BulletHandler(GameObject* obj);

	void onEvent(Event& e);
};

class EnemyHandler : public EventHandler {
public:
	GameObject* obj;
	EnemyHandler(GameObject* obj);

	void onEvent(Event& e);
};

class PlayerHandler : public EventHandler {
public:
	GameObject* obj;
	PlayerHandler(GameObject* obj);

	void onEvent(Event& e);
};

class GameTimeHandler : public EventHandler {
public:
	Timeline* timeline;
	Player* obj;

	GameTimeHandler(Timeline* timeline, Player* obj);
	
	void onEvent(Event& e);
};

class ReplayHandler : public EventHandler {
public:
	std::vector<float> initPositions;
	std::vector<int> objIndices;

	// probably an IO writing thing
	std::ofstream recordingOutput; // output
	std::ifstream replayInput;

	double replayStartTime;
	double replayEndTime;
	int eventCount;

	ReplayHandler();

	void onEvent(Event& e);
};

class ScriptHandler : public EventHandler {
public:
	ScriptManager* scripter;

	ScriptHandler(ScriptManager *scripter);

	void onEvent(Event& e);
};

class ConnectionHandler : public EventHandler {
private:
	zmq::socket_t* socket;
	int* currentGUID;
public:
	ConnectionHandler(zmq::socket_t* reqSocket, int* currentGUID);

	void onEvent(Event& e);
};