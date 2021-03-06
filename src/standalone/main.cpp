/*
   This is a sample single player client whose sole purpose is to demo what the
   Trogdor engine (in src/core) can do.  The engine itself supports multiple
   players.
*/

#include <memory>
#include <iostream>
#include <cstdlib>
#include <cstring>

#include <trogdor/game.h>
#include <trogdor/filesystem.h>
#include <trogdor/parser/parsers/xmlparser.h>
#include <trogdor/parser/parsers/inform7parser.h>

#include "include/streamout.h"
#include "include/streamerr.h"
#include "include/actions/timeaction.h"
#include "include/actions/quitaction.h"

#ifdef ENABLE_SERIALIZE_JSON
   #include "include/actions/saveaction.h"
   #include "include/actions/loadaction.h"
#endif


int main(int argc, char **argv) {

   // default game filename
   #ifdef GAME_XML_DEFAULT_PATH
      std::string gameXML = GAME_XML_DEFAULT_PATH;
   #else
      std::string gameXML = "game.xml";
   #endif

   if (argc > 2) {
      std::cerr << "Usage: trogdor [game_file]\n" << std::endl;
      return EXIT_FAILURE;
   }

   // user passed in a custom game filename
   if (argc > 1) {
	   gameXML = argv[1];
   }

   std::unique_ptr<trogdor::Game> currentGame = std::make_unique<trogdor::Game>(
      std::make_unique<StreamErr>(&std::cerr)
   );

   // The client can either choose from a parser type that's been implemented in
   // core or from a custom class that inherits from trogdor::Parser. Meanwhile,
   // the Game object is responsible for choosing an implementation of
   // trogdor::Instantiator. In the case of the standalone example, there are
   // only the two built-in parsers available: XML and Inform 7.
   std::unique_ptr<trogdor::Parser> parser;

   // We're going to naively use the file extension to decide which parser to
   // invoke
   std::string fileExtension = STD_FILESYSTEM::path(gameXML).extension();

   if (0 == trogdor::strToLower(fileExtension).compare(".xml")) {
      parser = std::make_unique<trogdor::XMLParser>(
         currentGame->makeInstantiator(),
         currentGame->getVocabulary(),
         currentGame->err()
      );
   }

   // We'll assume that all non-xml files must be Inform 7
   else {
      parser = std::make_unique<trogdor::Inform7Parser>(
         currentGame->makeInstantiator(),
         currentGame->getVocabulary(),
         currentGame->err()
      );
   }

   // The client can also implement its own custom game actions
   currentGame->insertVerbAction("time", std::make_unique<TimeAction>());
   currentGame->insertVerbAction("quit", std::make_unique<QuitAction>());

   #ifdef ENABLE_SERIALIZE_JSON
      currentGame->insertVerbAction("save", std::make_unique<SaveAction>());
      currentGame->insertVerbAction("load", std::make_unique<LoadAction>());
   #endif

   // The client can add its own synonyms for built-in verbs
   currentGame->insertVerbSynonym("escape", "quit");

   if (currentGame->initialize(parser.get(), gameXML, true)) {

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

      std::shared_ptr<trogdor::entity::Player> player = currentGame->createPlayer(
         "player",
         std::make_unique<StreamOut>(&std::cout),
         std::make_unique<StreamErr>(&std::cerr)
      );

      // The optional callback forces the user to acknowledge they've read the
      // introduction before they can start playing.
      currentGame->insertPlayer(player, [&player]() {

         std::string blah;

         player->out() << "\nPress enter to start." << std::endl;
         getline(std::cin, blah);
         player->out() << std::endl;
      });

      currentGame->start();

      while (currentGame->inProgress() && currentGame->playerIsInGame("player")) {

         std::string command;

         // Prompt the user for input
         currentGame->getPlayer("player")->out("prompt") << "\n> ";
         currentGame->getPlayer("player")->out("prompt").flush();

         getline(std::cin, command);
         currentGame->getPlayer("player")->input(command);
      }

      currentGame->stop();

      std::cout << "Game lasted for " << currentGame->getTime() + 1
         << " seconds.  Goodbye!\n" << std::endl;
   }

   return EXIT_SUCCESS;
}
