#include <memory>
#include <fstream>
#include <trogdor/entities/player.h>

#ifdef ENABLE_SERIALIZE_SQLITE
   #include <trogdor/serial/drivers/sqlite.h>
#else
   #include <trogdor/serial/drivers/json.h>
#endif

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

   // For now, we're defaulting to the SQLite3 driver if available
   #ifdef ENABLE_SERIALIZE_SQLITE
      trogdor::serial::Sqlite driver;
   #else
      trogdor::serial::Json driver;
   #endif

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
