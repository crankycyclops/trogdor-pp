#include "include/filesystem.h"


std::string Filesystem::getFullDefinitionsPath(std::string definitionPath) {

	if (STD_FILESYSTEM::path(definitionPath).is_absolute()) {
		return definitionPath;
	}

	else {

		std::string pathPrefix = TROGDORD_INSTALL_PREFIX;

		if (pathPrefix[pathPrefix.length() - 1] != STD_FILESYSTEM::path::preferred_separator) {
			pathPrefix += STD_FILESYSTEM::path::preferred_separator;
		}

		pathPrefix = pathPrefix + Config::get()->value<std::string>(Config::CONFIG_KEY_DEFINITIONS_PATH);

		if (pathPrefix[pathPrefix.length() - 1] != STD_FILESYSTEM::path::preferred_separator) {
			pathPrefix += STD_FILESYSTEM::path::preferred_separator;
		}

		return pathPrefix + definitionPath;
	}
}
