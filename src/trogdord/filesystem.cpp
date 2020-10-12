#include "include/filesystem.h"


std::string Filesystem::getFullDefinitionsPath(std::string definitionPath) {

	// If the game definition path we've passed in is already an absolute path,
	// just pass it through
	if (STD_FILESYSTEM::path(definitionPath).is_absolute()) {
		return definitionPath;
	}

	std::string basePath = Config::get()->value<std::string>(Config::CONFIG_KEY_DEFINITIONS_PATH);

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
