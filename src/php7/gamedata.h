#ifndef PHP_GAMEDATA_H
#define PHP_GAMEDATA_H

#include <queue>
#include <unordered_map>
#include <trogdor/game.h>

#include "message.h"

// Custom data that will be instantiated alongside the zend_object
struct customData {

	// The Game object, whether or not it's persistent, and it's persistent id
	trogdor::Game *obj;
	bool persistent;
	size_t id;

	// The Game's output buffer (mapped as Entity name => channel => output messages)
	std::unordered_map<
		std::string, std::unordered_map<
			std::string, std::queue<OutputMessage>
		>
	> outBuffer;
};

#endif /* PHP_GAMEDATA_H */
