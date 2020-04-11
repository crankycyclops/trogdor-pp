#ifndef INPUT_LISTENER_H
#define INPUT_LISTENER_H

#include <memory>

namespace input {


	// Abstract class that every input listener must inherit from
	class Listener {

		public:

			// Returns true if the listener has been started and false if not.
			virtual bool isStarted() = 0;

			// Starts the listener asynchronously and immediately returns.
			virtual void start() = 0;

			// Stops the listener. This method must block until all threads
			// associated with the listener have stopped. Not doing so will
			// result in undefined behavior.
			virtual void stop() = 0;
	};
}


#endif
