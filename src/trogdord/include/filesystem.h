#ifndef TROGDORD_FILESYSTEM_H
#define TROGDORD_FILESYSTEM_H

#include "config.h"
#include <trogdor/filesystem.h>


class Filesystem {

	public:

		// Returns true if the directory at the specified path is writable and
		// false if not.
		static bool isDirWritable(std::string path);

		// Utility function that takes as input a path that may or may not be
		// absolute. If it's absolute, the same path is returned unchanged. If
		// the path is relative, an absolute version is returned appended to
		// the trogdord installation prefix.
		static std::string getAbsolutePath(std::string path);

		// Utility function that takes as input a (possibly) relative path to
		// a game definition file and returns the full path. If the full path
		// is passed in as an argument, the same string is returned. By
		// default, if no path to a definition is specified, we return the
		// full path where game definitions are stored in the filesystem.
		static std::string getFullDefinitionsPath(std::string definitionPath = "");
};


#endif
