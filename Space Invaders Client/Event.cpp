#include "Event.h"
#include <chrono>

/**
* Gets current system time as a double.
*/
double Event::getCurrentTimeFromChronoAsDouble() {
	auto current_time = std::chrono::system_clock::now();
	auto duration_in_seconds = std::chrono::duration<double>(current_time.time_since_epoch());
	return duration_in_seconds.count();
}

Variant Event::getArg(std::string key) {
	return args[key];
}

void Event::addArg(std::string key, Variant value) {
	args[key] = value;
}

void Event::addArgPtr(std::string key, Variant* value) {
	args[key] = *value;
}

Event::Event(std::string eventType, int associatedGUID) {
	this->eventType = eventType;
	timestamp = getCurrentTimeFromChronoAsDouble();
	virtualTime = -1;
	this->associatedGUID = associatedGUID;
}

Event::Event(std::string eventType, int associatedGUID, double timestamp) {
	this->eventType = eventType;
	this->timestamp = timestamp;
	virtualTime = -1;
	this->associatedGUID = associatedGUID;
}

std::string Event::to_string() {
	std::string initString = "";
	initString.append(eventType).append(" "); // add the event type
	initString.append(std::to_string(associatedGUID)).append(" "); // add the associated GUID
	initString.append(std::to_string(timestamp)).append(" ");
	initString.append(std::to_string(args.size())).append(" ");

	std::map<std::string, Variant>::iterator i; // add the associated arguments
	for (i = args.begin(); i != args.end(); i++) {
		std::string argName = i->first;
		initString.append(argName).append(" ");

		Variant argVal = i->second;
		Variant::Type argType = argVal.type;
		
		if (argType == Variant::Type::BOOL) {
			initString.append(T_BOO).append(" ");
			initString.append(std::to_string(argVal.boolData)).append(" ");
		}
		else if (argType == Variant::Type::DOUBLE) {
			initString.append(T_DOU).append(" ");
			initString.append(std::to_string(argVal.doubleData)).append(" ");
		}
		else if (argType == Variant::Type::FLOAT) {
			initString.append(T_FLO).append(" ");
			initString.append(std::to_string(argVal.floatData)).append(" ");
		}
		else if (argType == Variant::Type::INTEGER) {
			initString.append(T_INT).append(" ");
			initString.append(std::to_string(argVal.intData)).append(" ");
		}
	}
	return initString;
}
