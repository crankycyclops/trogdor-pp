#ifndef OUTPUT_MESSAGE_H
#define OUTPUT_MESSAGE_H

#include <string>
#include <ctime>

namespace output {


	// Represents a single message to be inserted into the output buffer.
	struct Message {
		time_t timestamp;
		std::string content;
	};
}


#endif
