#ifndef INPUT_LISTENER_H
#define INPUT_LISTENER_H

#include <string>
#include <memory>

namespace input {


	// Abstract class that every input listener must inherit from
	class Listener {

		public:

			// Destructor
			virtual ~Listener();

			// Returns true if the listener has been started and false if not.
			virtual bool isStarted() = 0;

			// Starts the listener asynchronously and immediately returns.
			virtual void start() = 0;

			// Stops the listener. This method must block until all threads
			// associated with the listener have stopped. Not doing so will
			// result in undefined behavior.
			virtual void stop() = 0;

			// Takes as input a string received by the input listener, and if
			// the input is valid, sends it to the input driver. If the
			// input is invalid, it is ignored.
			void dispatch(std::string input);
	};
}


#endif
