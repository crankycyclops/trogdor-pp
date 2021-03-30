#include <memory>
#include <fstream>
#include <trogdor/entities/player.h>
#include <trogdor/exception/fileexception.h>

#ifdef ENABLE_SERIALIZE_SQLITE
   #include <trogdor/serial/drivers/sqlite.h>
#else
   #include <trogdor/serial/drivers/json.h>
#endif

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
   std::shared_ptr<trogdor::serial::Serializable> data;

   // For now, we're defaulting to the SQLite3 driver if available
   #ifdef ENABLE_SERIALIZE_SQLITE
      trogdor::serial::Sqlite driver;
   #else
      trogdor::serial::Json driver;
   #endif

   try {
      data = driver.deserializeFromDisk(filename);
   }

   catch (const trogdor::FileException &e) {
      player->out() << "Failed to load game state from " << filename << '.' << std::endl;
      return;
   }

   catch (const trogdor::Exception &e) {
      player->out() << e.what() << std::endl;
      return;
   }

   game->deserialize(
      data,
      [](trogdor::Game *game) -> std::unique_ptr<trogdor::Trogout> {
         return std::make_unique<StreamOut>(&std::cout);
      }, [](trogdor::Game *game) -> std::unique_ptr<trogdor::Trogerr> {
         return std::make_unique<StreamErr>(&std::cerr);
      }
   );

   game->getPlayer("player")->out() << "Game state loaded from " << filename << '.' << std::endl;
}
