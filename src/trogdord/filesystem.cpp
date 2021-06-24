#include <cstdio>
#include <trogdord/filesystem.h>


// Our quick and dirty method of determining whether or not a trogdord process
// can write to a directory is to determine if we can create and remove a test
// file inside of it.
bool Filesystem::isDirWritable(std::string path) {

	std::string filename = path + STD_FILESYSTEM::path::preferred_separator + ".trogtest";

	FILE *fp = std::fopen(filename.c_str(), "w");
	bool isWritable = fp == NULL ? false : true;

	if (isWritable) {
		std::remove(filename.c_str());
	}

	return isWritable;
}

/*****************************************************************************/

std::string Filesystem::getAbsolutePath(std::string path) {

	// If the configured path is relative, compute an absolute path based
	// on the installation prefix.
	if (STD_FILESYSTEM::path(path).is_relative()) {

		std::string prefix = TROGDORD_INSTALL_PREFIX;

		if (prefix[prefix.length() - 1] != STD_FILESYSTEM::path::preferred_separator) {
			prefix += STD_FILESYSTEM::path::preferred_separator;
		}

		path = prefix + path;
	}

	return path;
}

/*****************************************************************************/

std::string Filesystem::getFullDefinitionsPath(std::string definitionPath) {

	// If the game definition path we've passed in is already an absolute path,
	// just pass it through
	if (STD_FILESYSTEM::path(definitionPath).is_absolute()) {
		return definitionPath;
	}

	std::string basePath = Config::get()->getString(Config::CONFIG_KEY_DEFINITIONS_PATH);

	// Make sure, for consistency's sake, that absolute base paths always
	// end with a directory separator
	if (basePath[basePath.length() - 1] != STD_FILESYSTEM::path::preferred_separator) {
		basePath += STD_FILESYSTEM::path::preferred_separator;
	}

	if (!STD_FILESYSTEM::path(basePath).is_absolute()) {

		std::string pathPrefix = TROGDORD_INSTALL_PREFIX;

		if (pathPrefix[pathPrefix.length() - 1] != STD_FILESYSTEM::path::preferred_separator) {
			pathPrefix += STD_FILESYSTEM::path::preferred_separator;
		}

		basePath = pathPrefix + basePath;
	}

	return basePath + definitionPath;
}
