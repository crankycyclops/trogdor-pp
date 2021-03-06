#include <memory>
#include <trogdor/entities/player.h>

#include "../include/actions/timeaction.h"


/*
   Methods for the Time action.
*/
bool TimeAction::checkSyntax(const trogdor::Command &command) {

   // A valid quit command should only be one word, a verb
   if (command.getDirectObject().length() > 0 || command.getIndirectObject().length() > 0) {
      return false;
   }

   return true;
}


void TimeAction::execute(
   trogdor::entity::Player *player,
   const trogdor::Command &command,
   trogdor::Game *game
) {

   player->out("display") << "Current game time is " << game->getTime() << " seconds." << std::endl;
}
