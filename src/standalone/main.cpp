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


using namespace std;


int main(int argc, char **argv) {

   // default game filename
   string gameXML = "game.xml";

   if (argc > 3) {
	  cerr << "Usage: trogdor [game_file] [xml (default) | inform7]\n" << endl;
	  return EXIT_FAILURE;
   }

   // user passed in a custom game filename
   if (argc > 1) {
	  gameXML = argv[1];
   }

   std::unique_ptr<trogdor::Game> currentGame = make_unique<trogdor::Game>(
      make_unique<StreamOut>(&cerr)
   );

   // The client can either choose from a parser type that's been implemented in
   // core or from a custom class that inherits from trogdor::Parser. Meanwhile,
   // the Game object is responsible for choosing an implementation of
   // trogdor::Instantiator. In the case of the standalone example, there are
   // only the two built-in parsers available: XML and Inform 7.
   std::unique_ptr<trogdor::Parser> parser;

   // user wants to parse Inform 7 instead of XML, the default
   if (argc > 2 && 0 == strcmp("inform7", argv[2])) {
      parser = make_unique<trogdor::Inform7Parser>(
         std::move(currentGame->makeInstantiator()), currentGame->getVocabulary()
      );
   }

   else {
      parser = make_unique<trogdor::XMLParser>(
         std::move(currentGame->makeInstantiator()), currentGame->getVocabulary()
      );
   }

   // The client can add its own synonyms for built-in verbs
   currentGame->setSynonym("escape", "quit");

   // The client can also write and set its own custom game actions
   currentGame->setAction("time", std::make_unique<TimeAction>());

   if (currentGame->initialize(parser.get(), gameXML)) {

      // Demonstrates the retrieval of Game metadata
	  string title = currentGame->getMeta("title");
	  string author = currentGame->getMeta("author");

	  cout << endl;

	  if (title.length() > 0) {
	     cout << endl << "Title: " << title << endl;
	  }

	  if (author.length() > 0) {
	     cout << "Author: " << author << endl << endl;
	  }

	  Player *player = currentGame->createPlayer(
         "player",
         std::make_unique<StreamOut>(&cout),
         std::make_unique<StreamIn>(&cin),
         std::make_unique<StreamOut>(&cerr)
      );

	  currentGame->start();

	  while (currentGame->inProgress() && currentGame->playerIsInGame("player")) {
	     currentGame->processCommand(player);
	  }

	  currentGame->stop();

	  cout << "Game lasted for " << currentGame->getTime() + 1
	     << " seconds.  Goodbye!\n\n";
   }

   return EXIT_SUCCESS;
}

