#include <memory>
#include <fstream>
#include <trogdor/entities/player.h>

#include "../include/serial/json.h"
#include "../include/streamout.h"
#include "../include/streamerr.h"
#include "../include/actions/loadaction.h"


/*
   Methods for the Load action.
*/
bool LoadAction::checkSyntax(const trogdor::Command &command) {

   // A valid load command requires a filename.
   if (command.getDirectObject().length() > 0 && command.getIndirectObject().length() > 0) {
      return false;
   }

   else if (!command.getDirectObject().length() && !command.getIndirectObject().length()) {
      return false;
   }

   return true;
}


void LoadAction::execute(
   trogdor::entity::Player *player,
   const trogdor::Command &command,
   trogdor::Game *game
) {

   std::string filename = command.getDirectObject().length() ?
      command.getDirectObject() : command.getIndirectObject();

   ifstream inputFile(filename);

   if (!inputFile.is_open()) {
      player->out() << "Failed to load game state from " << filename << '.' << std::endl;
      return;
   }

   std::string buffer;
   ostringstream ss;

   ss << inputFile.rdbuf();
   buffer = ss.str();

   trogdor::serial::Json driver;
   std::shared_ptr<trogdor::serial::Serializable> data = driver.deserialize(buffer);

   game->deserialize(
      data,
      [](trogdor::Game *game) -> std::unique_ptr<trogdor::Trogout> {
         return std::make_unique<StreamOut>(&std::cout);
      }, [](trogdor::Game *game) -> std::unique_ptr<trogdor::Trogerr> {
         return std::make_unique<StreamErr>(&std::cerr);
      }
   );

   game->start();
   game->getPlayer("player")->out() << "Game state loaded from " << filename << '.' << std::endl;
}
