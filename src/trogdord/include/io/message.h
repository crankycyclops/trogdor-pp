#ifndef OUTPUT_MESSAGE_H
#define OUTPUT_MESSAGE_H

#include <string>
#include <ctime>


// Represents a single message to be inserted into the output buffer.
struct OutputMessage {
	time_t timestamp;
	std::string content;
};


#endif
