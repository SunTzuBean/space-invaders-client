#include "EventHandler.h"
#include "Player.h"
#include "EventManager.h"
#include "GameStateManager.h"


#include <mutex>

BulletHandler::BulletHandler(GameObject* obj) {
	this->obj = obj;
}

void BulletHandler::onEvent(Event& e) {
	if (e.eventType == POSITION_CHANGED) {
		float newXPos = e.getArg(X_POS).floatData;
		float newYPos = e.getArg(Y_POS).floatData;
		obj->setPosition(sf::Vector2f(newXPos, newYPos));
	}
	else if (e.eventType == COLLISION_PLAYER) {
		// raise an event that kills the player.
		EventManager* manager = manager->getInstance();
		Event p(PLAYER_DIED, -1, e.timestamp);
		// no arguments required.
		manager->raise(p);
		obj->kill();
	}
	else if (e.eventType == COLLISION_ENEMY) {
		int intendedGUID = e.getArg(INTENDED_GUID).intData;
		// raise an event that kills the enemy.
		EventManager* manager = manager->getInstance();
		Event p(ENEMY_DIED, intendedGUID, e.timestamp);
		manager->raise(p);
		obj->kill();
	}
	else if (e.eventType == COLLISION_BOUND) {
		// there are bounds at the top of the map that destroy the laser.
		// destory it if it collidied with this.
		obj->kill();
	}
	else if (e.eventType == MOVE) {
		// move it
		double yVel = e.getArg(Y_VEL).doubleData;
		double deltaT = e.getArg(DELTAT).doubleData;

		obj->move(0, yVel * deltaT);
	}
}

EnemyHandler::EnemyHandler(GameObject* obj) {
	this->obj = obj;
}

void EnemyHandler::onEvent(Event& e) {
	if (e.eventType == COLLISION_ENEMY) {
		// this means a bullet collided with an enemy.
		// kill this enemy.
		obj->kill();
	}
	else if (e.eventType == COLLISION_PLAYER) {
		// this means the player collided with an enemy.
		// raise an event that kills the player.
		EventManager* manager = manager->getInstance();
		Event p(PLAYER_DIED, -1, e.timestamp);
		manager->raise(p);
	}
	else if (e.eventType == ENEMY_DIED) {
		EventManager* manager = manager->getInstance();
		Event p(UPDATE_SCORE, -1, e.timestamp);
		manager->raise(p);
		obj->kill();
	}
	else if (e.eventType == SHOOT_LANE) {
		GameStateManager* state = state->getInstance();
		GameObject* bullet = new GameObject(ENEMY_BULLET);
		bullet->setPosition(sf::Vector2f(obj->getEnemy()->getPosition().x + ENEMY_WIDTH / 2, obj->getEnemy()->getPosition().y + ENEMY_HEIGHT));
		if (!state->addEnemyBullet(ENEMY_LANE_IDX, bullet)) { // if it wasn't able to add a bullet
			delete bullet;
		}
		else {
			GameStateManager* state = state->getInstance();
			state->registerEnemyBullets(ENEMY_BULLET_LANE_GUID);
		}
	}
	else if (e.eventType == SHOOT_RANDOM) {
		GameStateManager* state = state->getInstance();
		GameObject* bullet = new GameObject(ENEMY_BULLET);
		bullet->setPosition(sf::Vector2f(obj->getEnemy()->getPosition().x + ENEMY_WIDTH / 2, obj->getEnemy()->getPosition().y + ENEMY_HEIGHT));
		if (!state->addEnemyBullet(ENEMY_RAND_IDX, bullet)) { // if it wasn't able to add a bullet
			delete bullet;
		}
		else {
			GameStateManager* state = state->getInstance();
			state->registerEnemyBullets(ENEMY_BULLET_RAND_GUID);
		}
	}
	else if (e.eventType == MOVE_DOWN) {
		this->obj->move(0, 25);
	}
	else if (e.eventType == MOVE_LAT) {
		double xDist = e.getArg(X_DIST).doubleData;
		this->obj->move(xDist, 0);
	}
}


PlayerHandler::PlayerHandler(GameObject* obj) {
	this->obj = obj;
}

void PlayerHandler::onEvent(Event& e) {
	std::mutex mutex;
	std::lock_guard<std::mutex> lock(mutex);
	if (e.eventType == KEYBOARD_INPUT) {
		double newXVelocity = e.getArg(X_VEL).doubleData;
		double deltaT = e.getArg(DELTAT).doubleData;
		obj->getPlayer()->setXVelocity(newXVelocity);
		obj->move(deltaT * newXVelocity, 0);

		// don't let the player go out of bounds
		float playerXPos = obj->getPlayer()->getPosition().x;
		if (playerXPos < 0) {
			obj->move(0 - playerXPos, 0); // move the player right back
		}
		else if (playerXPos > 800 - 60) { // canvas width minus width of player (tacky I know)
			obj->move((800 - 60) - playerXPos, 0);
		}
	}
	else if (e.eventType == SHOOT) {
		GameStateManager* state = state->getInstance();
		GameObject* bullet = new GameObject(PLAYER_BULLET);
		bullet->setPosition(sf::Vector2f(obj->getPlayer()->getPosition().x + 30, obj->getPlayer()->getPosition().y - 45));
		if (!state->addPlayerBullet(bullet)) { // if it couldn't add a player bullet (because there's already one there
			delete bullet;
		}
		else { // if it could add a bullet, we need to have the game state register the new bullet.
			GameStateManager* state = state->getInstance();
			state->registerPlayerBullets(PLAYER_BULLET_GUID);
		}
	}
	else if (e.eventType == PLAYER_DIED) {
		// maybe we send a message to the server saying the player died.
		if (!(obj->getPlayer()->isInvincible())) { // if it's not invincible
			// do the stuff that would kill it and send a message
			obj->getPlayer()->setVisible(false);
			EventManager* manager = manager->getInstance();
			Event e(PLAYER_LOST_LIFE, -1, e.timestamp);
			manager->raise(e);
		} // otherwise do nothing.
	}
	
}

ConnectionHandler::ConnectionHandler(zmq::socket_t* reqSocket, int* currentGUID)
{
	this->socket = reqSocket;
	this->currentGUID = currentGUID;
}

void ConnectionHandler::onEvent(Event& e) {
	if (e.eventType == UPDATE_SCORE) {
		std::string message = ADD_POINTS;
		socket->send(zmq::buffer(message), zmq::send_flags::none);
		// the server handles all of this, so we don't expect a reply.
		zmq::message_t dummy;
		socket->recv(dummy);
	}
	else if (e.eventType == PLAYER_LOST_LIFE) {
		std::string message = PLAYER_DEAD;
		socket->send(zmq::buffer(message), zmq::send_flags::none);
		// the server may say that the game is over or something, TODO

		zmq::message_t reply;
		socket->recv(std::ref(reply), zmq::recv_flags::none); // the game waits while the server sleeps to send a message.
		std::string info = std::string(static_cast<char*>(reply.data()), reply.size());
		int lives = std::stoi(info);

		if (lives == 0) {
			exit(0);
		}

		GameStateManager* state = state->getInstance();
		state->makePlayerVisible();
	}
	else if (e.eventType == WON) {
		std::string message = WINNER;
		socket->send(zmq::buffer(message), zmq::send_flags::none);

		zmq::message_t reply;
		socket->recv(std::ref(reply), zmq::recv_flags::none); // the game waits while the server sleeps to send a message.
		exit(0);
	}
}



GameTimeHandler::GameTimeHandler(Timeline* timeline, Player* obj) {
	this->timeline = timeline;
	this->obj = obj;
}

void GameTimeHandler::onEvent(Event& e) {
	std::mutex mutex;
	std::lock_guard<std::mutex> lock(mutex);
	if (e.eventType == PAUSED) {
		timeline->pause();
	}
	else if (e.eventType == UNPAUSED) {
		timeline->unpause();
	}
	else if (e.eventType == CHANGED_TIMESCALE) {
		double newScale = e.getArg(SCALE).doubleData;
		timeline->changeTick(newScale);
		std::cout << "new timescale = " << newScale << std::endl;

		EventManager* manager = manager->getInstance();
		Event p(INFO, -1, e.timestamp);

		Variant v;
		v.type = Variant::Type::INTEGER;
		v.intData = S_TIME;
		p.addArg(MESSAGE, v);
		manager->raise(p);
	}
}

ReplayHandler::ReplayHandler() {
	this->initPositions = {};
	this->objIndices = {};
	// writing and reading tool init here
	recordingOutput.open("recording.txt");
	replayStartTime = 0;
	replayEndTime = 0;
	eventCount = 0;
	
}

void ReplayHandler::onEvent(Event& e) {
	std::mutex mutex;
	std::lock_guard<std::mutex> lock(mutex);
	if (e.eventType == BEGIN_REPLAY) {
		recordingOutput.close();
		replayInput.open("recording.txt");
		EventManager* manager = manager->getInstance();
		manager->isRecording = false; // we are no longer recording any events. They will be queued and played back.
		manager->recordingEndTime = e.timestamp;

		// if we got this, it's time to raise a lot of events.
		//std::cout << "the file is open: " << recordingOutput.is_open() << std::endl;
		// now load in the initial positions of everything.
		/**
		std::map<int, sf::Vector2f>::iterator i;
		for (i = manager->positionsMap.begin(); i != manager->positionsMap.end(); i++) {
			Event initialPosEvent(POSITION_CHANGED, i->first, 0); // must be handled first.
			Variant x;
			Variant y;

			x.type = Variant::Type::FLOAT;
			x.floatData = i->second.x;
			y.type = Variant::Type::FLOAT;
			y.floatData = i->second.y;

			initialPosEvent.addArg(X_POS, x);
			initialPosEvent.addArg(Y_POS, y);
			manager->raise(initialPosEvent);
		}
		*/
		
		// I'm not sure how to ensure they'll be handled yet.
		// operator >> gets a new line from the input stream.
		//recording.clear();
		replayInput.seekg(0, std::ios::beg); // this should return the cursor to the beginning of the file, ready for reading.
		std::string currEvent;
		while (std::getline(replayInput, currEvent)) {
			// pick apart the string similarly to how I get stuff from main
			// TODO finish this!

			std::istringstream bootlegScanner(currEvent);
			std::string currentString;
			int currentInt;
			float currentFloat;
			bool currentBool;
			double currentDouble;

			// first thing is the event's type.
			bootlegScanner >> currentString;
			std::string eventType = currentString;

			// next is the associated guid.
			bootlegScanner >> currentInt;
			int assocGUID = currentInt;

			// next is the timestamp.
			bootlegScanner >> currentDouble;
			double timestamp = currentDouble;

			// now we can construct an event.
			Event replayEvent(eventType, assocGUID, timestamp);

			// next is the number of arguments the event had.
			bootlegScanner >> currentInt;
			int numOfArgs = currentInt;
			
			// now time to collect the arguments.
			for (int i = 0; i < numOfArgs; i++) {
				Variant v;

				// the first thing to collect is a string that says what the name of the argument is.
				bootlegScanner >> currentString;
				std::string argName = currentString;

				// next is the type of the next piece of data.
				bootlegScanner >> currentString;
				if (currentString == T_INT) {
					v.type = Variant::Type::INTEGER;

					bootlegScanner >> currentInt;
					v.intData = currentInt;
					replayEvent.addArg(argName, v);
				}
				else if (currentString == T_FLO) {
					v.type = Variant::Type::FLOAT;

					bootlegScanner >> currentFloat;
					v.floatData = currentFloat;
					replayEvent.addArg(argName, v);
				}
				else if (currentString == T_BOO) {
					v.type = Variant::Type::BOOL;

					bootlegScanner >> currentBool;
					v.boolData = currentBool;
					replayEvent.addArg(argName, v);
				}
				else if (currentString == T_DOU) {
					v.type = Variant::Type::DOUBLE;
					
					bootlegScanner >> currentDouble;
					v.doubleData = currentDouble;
					replayEvent.addArg(argName, v);
				}
				
			}
			// now, the event is fully created and ready to be raised.
			manager = manager->getInstance();
			manager->raise(replayEvent);
		}
		manager->isReplaying = true; // the replay is officially playing now!
		replayInput.close();
		recordingOutput.open("recording.txt"); // open for recording for next time
	}
	else if (e.eventType != BEGIN_REC && e.eventType != END_REC) {
			std::string stringRep = e.to_string();
			recordingOutput << stringRep << std::endl;
	}
		
}

void printStatus(std::string status) {
	std::cout << status << std::endl;
}

ScriptHandler::ScriptHandler(ScriptManager *scripter) {
	this->scripter = scripter;
}

void ScriptHandler::onEvent(Event& e) {
	if (e.eventType == INFO) {
		int status = e.getArg(MESSAGE).intData;
		// raise to script that handles this type of event
		
		dukglue_register_function(scripter->getContext(), &printStatus, "printStatus"); // allows for the script to print here.

		scripter->loadScript("script.js");
		scripter->runScript("handleStatus", T_INT, 1, status); // runs the script
		/**		Variant v = e.getArg(MESSAGE);
		int status = v.intData;
		if (status == S_DIED) {
			ScriptManager scriptManager = ScriptManager();
			dukglue_register_method(scriptManager.getContext());
		}
		*/
	}
}