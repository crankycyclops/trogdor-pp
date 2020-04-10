#ifndef INPUT_DRIVER_H
#define INPUT_DRIVER_H

#include <string>
#include <memory>
#include <optional>
#include <unordered_map>
#include <mutex>

namespace input {


	struct InputBuffer {

		// True only if the buffer has been given a value
		bool isSet = false;

		// Contents of the input buffer
		std::string buffer;
	};

	/************************************************************************/

	class Driver {

		private:

			// Singleton instance of input::Driver
			static std::unique_ptr<Driver> instance;

			// Constructor should only be called internally by get(), which
			// will ensure we only ever have a single instance of the class.
			Driver();
			Driver(const Driver &) = delete;

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

			// Returns singleton instance of Driver
			static std::unique_ptr<Driver> &get();

			// Returns true if an entity's input buffer contains a string and
			// false if not.
			bool isSet(
				size_t gameId,
				std::string entityName
			);

			// Sets the contents of an entity's input buffer. If called more
			// than once without calling get(), the buffer will be overwritten.
			void set(
				size_t gameId,
				std::string entityName,
				std::string input
			);

			// Pops the contents of an entity's input buffer. If nothing is in
			// the buffer when this method is called, nothing will be returned.
			std::optional<std::string> consume(
				size_t gameId,
				std::string entityName
			);
	};
}


#endif
