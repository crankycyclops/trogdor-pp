#include <trogdor/parser/parsers/xmlparser.h>

#include "include/config.h"
#include "include/filesystem.h"
#include "include/exception/serverexception.h"

#include "include/gamewrapper.h"


GameWrapper::GameWrapper(
	std::string definitionPath,
	std::string name,
	std::unordered_map<std::string, std::string> meta
): name(name), definition(definitionPath) {

	definitionPath = Filesystem::getFullDefinitionsPath(definitionPath);

	// TODO: will need better and more specific error logging than just a
	// simple copy of the global server error logger
	gamePtr = std::make_unique<trogdor::Game>(
		Config::get()->err().copy()
	);

	std::unique_ptr<trogdor::XMLParser> parser = std::make_unique<trogdor::XMLParser>(
		gamePtr->makeInstantiator(), gamePtr->getVocabulary()
	);

	if (!gamePtr->initialize(parser.get(), definitionPath)) {
		throw ServerException("failed to initialize game");
	}

	// Record when the game was created
	created = std::time(nullptr);

	// If any custom meta data was specified, set it
	for (auto &pair: meta) {
		gamePtr->setMeta(pair.first, pair.second);
	}
}
