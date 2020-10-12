#include "include/filesystem.h"


std::string Filesystem::getFullDefinitionsPath(std::string definitionPath) {

	std::string basePath = Config::get()->value<std::string>(Config::CONFIG_KEY_DEFINITIONS_PATH);

	if (!definitionPath.length() && STD_FILESYSTEM::path(basePath).is_absolute()) {

		// Make sure, for consistency's sake, that absolute base paths always
		// end with a directory separator
		if (basePath[basePath.length() - 1] != STD_FILESYSTEM::path::preferred_separator) {
			basePath += STD_FILESYSTEM::path::preferred_separator;
		}

		return basePath;
	}

	else if (STD_FILESYSTEM::path(definitionPath).is_absolute()) {
		return definitionPath;
	}

	else {

		std::string pathPrefix = TROGDORD_INSTALL_PREFIX;

		if (pathPrefix[pathPrefix.length() - 1] != STD_FILESYSTEM::path::preferred_separator) {
			pathPrefix += STD_FILESYSTEM::path::preferred_separator;
		}

		pathPrefix = pathPrefix + basePath;

		if (pathPrefix[pathPrefix.length() - 1] != STD_FILESYSTEM::path::preferred_separator) {
			pathPrefix += STD_FILESYSTEM::path::preferred_separator;
		}

		// If we're returning the base path for a configuration value that was
		// relative, definitionPath will be blank, and so we'll still get a
		// correct result.
		return pathPrefix + definitionPath;
	}
}
