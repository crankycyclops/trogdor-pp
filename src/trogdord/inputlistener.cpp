#include "include/config.h"
#include "include/io/input/driver.h"
#include "include/io/iostream/serverin.h"
#include "include/inputlistener.h"


InputListener::~InputListener() {

	stop();
}

/*****************************************************************************/

void InputListener::_subscribe(trogdor::entity::Player *pPtr) {

	processed[pPtr->getName()].playerPtr = pPtr;
	processed[pPtr->getName()].playerName = pPtr->getName();
}

/*****************************************************************************/

void InputListener::_unsubscribe(
	std::string playerName,
	std::function<void()> afterProcessCommand
) {

	// Do the actual removal in the worker thread after we're sure we're not
	// waiting on anymore commands.
	if (processed.end() != processed.find(playerName)) {

		trogdor::entity::Player *pPtr = processed[playerName].playerPtr;

		if (pPtr) {

			// If specified, this is the callback that should be fired by the
			// async task after game->processCommand() returns.
			if (afterProcessCommand) {
				afterCommandCallbacks[pPtr] = afterProcessCommand;
			}

			processed[playerName].playerPtr = nullptr;
			static_cast<ServerIn &>(pPtr->in()).kill();
		}
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
			_subscribe(static_cast<trogdor::entity::Player *>(player.second.get()));
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

				// Can't use a range-based for here because I need to be able
				// to alter the unordered map in the case where an
				// unsubscribed player needs to be removed.
				for (auto it = processed.begin(); it != processed.end(); ) {

					// If a command for this player has already been
					// processed, or if this is the first time the thread
					// is executed, process the next command.
					if (!it->second.initialized || it->second.isReady()) {

						if (it->second.playerPtr) {

							it->second.future = std::async(
								std::launch::async,
								[&](trogdor::entity::Player *pPtr) -> bool {

									gamePtr->processCommand(pPtr);

									// We might have been given a callback to
									// execute after processCommand() returns.
									if (afterCommandCallbacks.end() != afterCommandCallbacks.find(pPtr)) {
										afterCommandCallbacks[pPtr]();
										afterCommandCallbacks.erase(pPtr);
									}

									return true;
								},
								it->second.playerPtr
							);

							it->second.initialized = true;
							it++;
						}

						// The player was removed from the game, so stop
						// listening for their commands.
						else {

							if (it->second.future.valid()) {
								it->second.future.wait();
							}

							it = processed.erase(it);
						}
					}

					else {
						it++;
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

		processedMutex.lock();

		// Signal to the worker thread that we shouldn't listen for commands
		// from the players anymore.
		for (auto &next: processed) {
			_unsubscribe(next.second.playerName);
		}

		// After signaling to the worker that we shouldn't listen for player
		// commands anymore, wait for the async tasks doing the listening to
		// fall out of scope.
		for (auto &next: processed) {
			if (next.second.future.valid()) {
				next.second.future.wait();
			}
		}

		// Finally, kill the worker thread.
		on = false;

		processedMutex.unlock();
		worker.join();
	}
}
