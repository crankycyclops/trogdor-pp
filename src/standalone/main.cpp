/*
   This is a sample single player client whose sole purpose is to demo what the
   Trogdor engine (in src/core) can do.  The engine itself supports multiple
   players.
*/

#include <memory>
#include <iostream>
#include <cstdlib>
#include <cstring>

#include "../core/include/game.h"
#include "../core/include/parser/parsers/xmlparser.h"
#include "../core/include/parser/parsers/inform7parser.h"

#include "include/streamout.h"
#include "include/streamin.h"
#include "include/actions/timeaction.h"


int main(int argc, char **argv) {

   // default game filename
   std::string gameXML = "game.xml";

   if (argc > 3) {
	  std::cerr << "Usage: trogdor [game_file] [xml (default) | inform7]\n" << std::endl;
	  return EXIT_FAILURE;
   }

   // user passed in a custom game filename
   if (argc > 1) {
	  gameXML = argv[1];
   }

   std::unique_ptr<trogdor::Game> currentGame = std::make_unique<trogdor::Game>(
      std::make_unique<StreamOut>(&std::cerr)
   );

   // The client can either choose from a parser type that's been implemented in
   // core or from a custom class that inherits from trogdor::Parser. Meanwhile,
   // the Game object is responsible for choosing an implementation of
   // trogdor::Instantiator. In the case of the standalone example, there are
   // only the two built-in parsers available: XML and Inform 7.
   std::unique_ptr<trogdor::Parser> parser;

   // user wants to parse Inform 7 instead of XML, the default
   if (argc > 2 && 0 == strcmp("inform7", argv[2])) {
      parser = std::make_unique<trogdor::Inform7Parser>(
         currentGame->makeInstantiator(), currentGame->getVocabulary()
      );
   }

   else {
      parser = std::make_unique<trogdor::XMLParser>(
         currentGame->makeInstantiator(), currentGame->getVocabulary()
      );
   }

   // The client can add its own synonyms for built-in verbs
   currentGame->insertVerbSynonym("escape", "quit");

   // The client can also write and set its own custom game actions
   currentGame->insertVerbAction("time", std::make_unique<TimeAction>());

   if (currentGame->initialize(parser.get(), gameXML)) {

      // Demonstrates the retrieval of Game metadata
	  std::string title = currentGame->getMeta("title");
	  std::string author = currentGame->getMeta("author");

	  std::cout << std::endl;

	  if (title.length() > 0) {
	     std::cout << std::endl << "Title: " << title << std::endl;
	  }

	  if (author.length() > 0) {
	     std::cout << "Author: " << author << std::endl << std::endl;
	  }

	  Player *player = currentGame->createPlayer(
         "player",
         std::make_unique<StreamOut>(&std::cout),
         std::make_unique<StreamIn>(&std::cin),
         std::make_unique<StreamOut>(&std::cerr)
      );

	  currentGame->start();

	  while (currentGame->inProgress() && currentGame->playerIsInGame("player")) {
	     currentGame->processCommand(player);
	  }

	  currentGame->stop();

	  std::cout << "Game lasted for " << currentGame->getTime() + 1
	     << " seconds.  Goodbye!\n" << std::endl;
   }

   return EXIT_SUCCESS;
}
