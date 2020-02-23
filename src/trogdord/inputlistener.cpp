#include "include/inputlistener.h"


InputListener::~InputListener() {

	stop();
}

/*****************************************************************************/

void InputListener::_subscribe(trogdor::entity::Player *pPtr, bool lock) {

	PlayerFuture pf;

	if (lock) {
		processedMutex.lock();
	}

	pf.playerPtr = pPtr;
	processed[pPtr->getName()] = std::move(pf);

	if (lock) {
		processedMutex.unlock();
	}
}

/*****************************************************************************/

void InputListener::_unsubscribe(std::string playerName, bool lock) {

	if (lock) {
		processedMutex.lock();
	}

	// Do the actual removal in the worker thread after we're sure we're not
	// waiting on anymore commands.
	if (processed.end() != processed.find(playerName)) {
		processed[playerName].playerPtr = nullptr;
	}

	if (lock) {
		processedMutex.unlock();
	}
}

/*****************************************************************************/

void InputListener::start() {

	if (!on) {

		on = true;

		// Initialize the worker thread with a list of players whose commands
		// need to be listened for and processed.
		processedMutex.lock();

		for (const auto &player: gamePtr->getPlayers()) {
			_subscribe(static_cast<trogdor::entity::Player *>(player.second.get()), false);
		}

		processedMutex.unlock();

		// Worker thread that will listen for and process player commands.
		worker = std::thread([&]() {

			// How long to sleep the thread before polling again
			static std::chrono::milliseconds sleepTime(
				InputListener::THREAD_SLEEP_MS
			);

			while (on) {

				processedMutex.lock();

				for (auto &next: processed) {

					// If a command for this player has already been
					// processed, or if this is the first time the thread
					// is executed, process the next command.
					if (!next.second.initialized || next.second.isReady()) {

						if (next.second.playerPtr) {

							next.second.future = std::async(
								std::launch::async,
								[&](trogdor::entity::Player *pPtr) -> bool {
									gamePtr->processCommand(pPtr);
									return true;
								},
								next.second.playerPtr
							);

							next.second.initialized = true;
						}

						// The player was removed from the game, so stop
						// listening for their commands.
						else {
							// TODO: send null command
							next.second.future.wait();
							processed.erase(next.first);
						}
					}
				}

				processedMutex.unlock();
				std::this_thread::sleep_for(sleepTime);
			}
		});
	}
}

/*****************************************************************************/

void InputListener::stop() {

	if (on) {
		on = false;
		worker.join();
	}
}
