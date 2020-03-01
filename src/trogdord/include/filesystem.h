#ifndef TROGDORD_FILESYSTEM_H
#define TROGDORD_FILESYSTEM_H

#include "config.h"
#include <trogdor/filesystem.h>


class Filesystem {

	public:

		// Utility function that takes as input a (possibly) relative path to
		// a game definition file and returns the full path. If the full path
		// is passed in as an argument, the same string is returned. By
		// default, if no path to a definition is specified, we return the
		// full path where game definitions are stored in the filesystem.
		static std::string getFullDefinitionsPath(std::string definitionPath = "");
};


#endif
