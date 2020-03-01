#ifndef INPUT_REDIS_H
#define INPUT_REDIS_H

#include "driver.h"


namespace input {


	class Redis: public Driver {

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
