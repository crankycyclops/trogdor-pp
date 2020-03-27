#include <string>
#include "utility.h"


bool strReplace(std::string &str, const std::string &substr, const std::string &replacement) {

	size_t start = str.find(substr);

	if (start == std::string::npos) {
		return false;
	}

	str.replace(start, substr.length(), replacement);
	return true;
}
