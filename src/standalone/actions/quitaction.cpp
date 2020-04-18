#include <memory>
#include "../include/actions/quitaction.h"


/*
   Methods for the Quit action.
*/
bool QuitAction::checkSyntax(const std::shared_ptr<trogdor::Command> &command) {

   // A valid quit command should only be one word, a verb
   if (command->getDirectObject().length() > 0 || command->getIndirectObject().length() > 0) {
      return false;
   }

   return true;
}


void QuitAction::execute(Player *player,
const std::shared_ptr<trogdor::Command> &command, trogdor::Game *game) {

   game->removePlayer(player->getName());
}
