#ifndef INPUT_LISTENER_CONTAINER_H
#define INPUT_LISTENER_CONTAINER_H

#include <memory>
#include <unordered_map>

#include "listeners/listener.h"

namespace input {


	// Manages one or more instances of input::Listener
	class ListenerContainer {

		private:

			// Singleton instance of input::ListenerContainer
			static std::unique_ptr<ListenerContainer> instance;

			// Contains all configured event listeners (mapped by name)
			std::unordered_map<std::string, std::unique_ptr<Listener>> listeners;

			// Constructor should only be called internally by get(), which
			// will ensure we only ever have a single instance of the class.
			ListenerContainer();
			ListenerContainer(const ListenerContainer &) = delete;

		public:

			// Returns singleton instance of Driver
			static std::unique_ptr<ListenerContainer> &get();

			// Destructor makes sure all input listeners are shut down
			// properly when the singleton instance falls out of scope
			~ListenerContainer();
	};
}


#endif
