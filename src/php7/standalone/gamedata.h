#ifndef PHP_GAMEDATA_H
#define PHP_GAMEDATA_H

#include <queue>
#include <thread>
#include <unordered_map>
#include <condition_variable>
#include <trogdor/game.h>

#include "message.h"

// Custom data associated with a game that can't be a part of customData below
// because it's allocated with ecalloc() and therefore won't have the
// constructors called. Instead, we'll insert this into an unordered_map whose
// keys will be of type trogdor::Game *.
struct customConstructedData {

	// Each time a player is inserted into the game, a thread is created to
	// execute that player's commands.
	std::unordered_map<
		trogdor::entity::Player *,
		std::thread,
		std::hash<trogdor::entity::Player *>
	> playerThreads;

	// PHPStreamIn will use these two variables to make the thread that calls
	// it wait until there's a value in the input buffer.
	std::mutex inputConditionMutex;
	std::condition_variable inputCondition;

	// We need to lock on these mutexes whenever we access or change inBuffer
	// and outBuffer.
	std::mutex inBufferMutex;
	std::mutex outBufferMutex;

	// The game's input buffer, a queue of commands sent by an Entity that are
	// then consumed by a PHPStreamIn instance (mapped as Entity name => input commands)
	std::unordered_map<std::string, std::queue<std::string>> inBuffer;

	// The Game's output buffer (mapped as Entity name => channel => output messages)
	std::unordered_map<
		std::string, std::unordered_map<
			std::string, std::queue<OutputMessage>
		>
	> outBuffer;
};

/*****************************************************************************/

// Custom data that will be instantiated alongside the zend_object
struct customData {

	// The Game object, whether or not it's persistent, and it's persistent id
	trogdor::Game *obj;
	bool persistent;
	size_t id;
};

/*****************************************************************************/

// Maps trogdor::Game * to a struct containing data that can't be stored in
// customData.
extern std::unordered_map<
	trogdor::Game *,
	std::unique_ptr<customConstructedData>,
	std::hash<trogdor::Game *>
> customConstructedDataMap;

#endif /* PHP_GAMEDATA_H */
