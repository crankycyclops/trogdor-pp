#include <memory>
#include <fstream>
#include <trogdor/entities/player.h>

#include "../include/serial/json.h"
#include "../include/actions/saveaction.h"


/*
   Methods for the Save action.
*/
bool SaveAction::checkSyntax(const trogdor::Command &command) {

   // A valid save command requires a filename.
   if (command.getDirectObject().length() > 0 && command.getIndirectObject().length() > 0) {
      return false;
   }

   else if (!command.getDirectObject().length() && !command.getIndirectObject().length()) {
      return false;
   }

   return true;
}


void SaveAction::execute(
   trogdor::entity::Player *player,
   const trogdor::Command &command,
   trogdor::Game *game
) {

   std::string filename = command.getDirectObject().length() ?
      command.getDirectObject() : command.getIndirectObject();

   trogdor::serial::Json driver;
   std::shared_ptr<trogdor::serial::Serializable> data = game->serialize();

   ofstream outputFile(filename);

   if (outputFile.is_open()) {
      outputFile << std::any_cast<std::string>(driver.serialize(data));
      outputFile.close();
      player->out() << "Game state saved to " << filename << '.' << std::endl;
   } else {
      player->out() << "Failed to save game state to " << filename << '.' << std::endl;
   }
}
