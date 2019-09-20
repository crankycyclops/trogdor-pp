#include <memory>
#include "../include/actions/timeaction.h"

using namespace std;


/*
  Methods for the Time action.
*/
bool TimeAction::checkSyntax(const std::shared_ptr<trogdor::core::Command> &command) {

   // A valid quit command should only be one word, a verb
   if (command->getDirectObject().length() > 0 ||
      command->getIndirectObject().length() > 0) {
      return false;
   }

   return true;
}


void TimeAction::execute(trogdor::core::entity::Player *player,
const std::shared_ptr<trogdor::core::Command> &command, trogdor::core::Game *game) {

   player->out("display") << "Current game time is " << game->getTime() << " seconds." << endl;
}

