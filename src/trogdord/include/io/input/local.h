#ifndef INPUT_LOCAL_H
#define INPUT_LOCAL_H

#include <mutex>
#include "driver.h"


namespace input {


	struct InputBuffer {

		// True inly if the buffer has been given a value
		bool isSet = false;

		// Contents of the input buffer
		std::string buffer;
	};

	/************************************************************************/

	class Local: public Driver {

		private:

			// Make sure all operations on the local input buffer are thread-safe
			std::mutex bufferMutex;

			// Game ID -> Entity name -> Input Buffer
			std::unordered_map<size_t,
				std::unordered_map<std::string, InputBuffer>
			> inputBuffers;

			// Returns true if an input buffer already exists for the
			// game ID, entity name pair and false if not.
			inline bool bufferExists(size_t gameId, std::string entityName) {

				if (inputBuffers.end() == inputBuffers.find(gameId)) {
					return false;
				}

				else if (inputBuffers[gameId].end() == inputBuffers[gameId].find(entityName)) {
					return false;
				}

				else {
					return true;
				}
			}

		public:

			// String representation of the driver's name.
			static const char *DRIVER_NAME;

			// Returns true if an entity's input buffer contains a string and
			// false if not.
			virtual bool isSet(
				size_t gameId,
				std::string entityName
			);

			// Sets the contents of an entity's input buffer. If called more
			// than once without calling get(), the buffer will be overwritten.
			virtual void set(
				size_t gameId,
				std::string entityName,
				std::string input
			);

			// Pops the contents of an entity's input buffer. If nothing is in
			// the buffer when this method is called, nothing will be returned.
			virtual std::optional<std::string> consume(
				size_t gameId,
				std::string entityName
			);
	};
}


#endif
