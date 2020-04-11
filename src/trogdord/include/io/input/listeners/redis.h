#ifndef INPUT_LISTENER_REDIS_H
#define INPUT_LISTENER_REDIS_H

#include "listener.h"

namespace input {


	class Redis: public Listener {

		private:

			// Set to true if the listener has been started
			bool started = false;

		public:

			// String representation of the listener's name.
			static const char *LISTENER_NAME;

			// Returns true if the listener has been started and false if not.
			virtual bool isStarted();

			// Starts the listener asynchronously and immediately returns.
			virtual void start();

			// Stops the listener. This method must block until all threads
			// associated with the listener have stopped. Not doing so will
			// result in undefined behavior.
			virtual void stop();
	};
}


#endif
