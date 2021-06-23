#include <string>
#include "utility.h"


bool strReplace(std::string &str, const std::string &substr, const std::string &replacement) {

	bool matched = false;

	for (size_t i = str.find(substr); i != std::string::npos; i = str.find(substr, i + replacement.length())) {
		str.replace(i, substr.length(), replacement);
		matched = true;
	}

	return matched;
}
