#pragma once
#include <string>
#include <map>

/**These are all event types, as strings. */
#define POSITION_CHANGED "POSITION_CHANGED"
#define VELOCITY_CHANGED "VELOCITY_CHANGED"
#define SPAWNED "SPAWNED"
#define DIED "DIED"
#define CHANGED_TIMESCALE "CHANGED_TIMESCALE"
#define PAUSED "PAUSED"
#define UNPAUSED "UNPAUSED"
#define COLLISION "COLLISION"
#define NEW_CLIENT "NEW_CLIENT"
#define TRANSPOSE "TRANSPOSE"
#define JUMPED "JUMPED"
#define BEGIN_REC "BEGIN_REC"
#define SET_INIT_POS "SET_INIT_POS"
#define BEGIN_REPLAY "BEGIN_REPLAY"
#define END_REC "END_REC"
#define FRICTION "FRICTION"
#define GRAVITY "GRAVITY"
#define LATERAL_INPUT "LATERAL_INPUT"
#define ADJUST_OFFSET "ADJUST_OFFSET"
#define INFO "INFO"
// collision with bullet -> player
#define COLLISION_PLAYER "COLLISION_PLAYER" 
// collision with bullet -> enemy
#define COLLISION_ENEMY "COLLISION_ENEMY"
// bullet collisions w/ outside boundary
#define COLLISION_BOUND "COLLISION_BOUND"
#define PLAYER_DIED "PLAYER_DIED"
#define ENEMY_DIED "ENEMY_DIED"
#define SHOOT "SHOOT"
#define SHOOT_LANE "SHOOT_LANE"
#define SHOOT_RANDOM "SHOOT_RANDOM"
#define KEYBOARD_INPUT "KEYBOARD_INPUT"
#define MOVE_DOWN "MOVE_DOWN"
#define MOVE_LAT "MOVE_LAT"
#define MOVE "MOVE"
#define UPDATE_SCORE "UPDATE_SCORE"
#define PLAYER_LOST_LIFE "PLAYER_LOST_LIFE"
#define WON "WON"



/** These are the types of event arguments. */
#define X_POS "xPos"
#define Y_POS "yPos"
#define X_VEL "xVel"
#define Y_VEL "yVel"
#define ID "id"
#define X_DIST "xDist"
#define Y_DIST "yDist"
#define COLLISION_TYPE "collisionType"
#define PAUSED_STATUS "pausedStatus"
#define OBJ_OFFSET "objOffset"
#define PLAY_OFFSET "playOffset"
#define FROM_LEFT "fromLeft"
#define SCALE "scale"
#define MESSAGE "message"
#define INTENDED_GUID "intendedGUID"
#define DELTAT "deltaT"
#define PTS "points"

/** types as macros to ensure continuity.*/
#define T_INT "int"
#define T_FLO "float"
#define T_BOO "bool"
#define T_DOU "double"

/** "enum" spoof for event types when updating status */
#define S_DIED 0
#define S_SPAWNED 1
#define S_TIME 2
/**
* Taken from textbook.
*/
class Variant {
public:
	enum class Type {
		INTEGER,
		FLOAT,
		BOOL,
		DOUBLE
	};
	Type type;
	union {
		int intData;
		float floatData;
		bool boolData;
		double doubleData;
	};
	
	/*void setData(float f) {
		if (type == Variant::Type::FLOAT) {
			floatData = f;
		}
	}*/
};

class Event {

public:
	std::string eventType;

	/** The timestamp when the event was raised/created. */
	double timestamp;
	/** The virtual time to sort the events, will use later eventually. */
	int virtualTime;

	int associatedGUID;

	std::map<std::string, Variant> args; // args for event.

	Event(std::string eventType, int associatedGUID);

	Event(std::string eventType, int associatedGUID, double timestamp);

	Variant getArg(std::string key);
	void addArg(std::string key, Variant value);

	void addArgPtr(std::string key, Variant* value);

	double getCurrentTimeFromChronoAsDouble();

	std::string to_string();
};

