#ifndef TIMEACTION_H
#define TIMEACTION_H


#include <memory>
#include "../../../core/include/action.h"

using namespace std;


/*
  TimeAction outputs the current game time to whichever player issues
  the command.
*/
class TimeAction: public trogdor::core::Action {

   public:

      /*
         See documentation in core/include/action.h.  A valid syntax for the
         Time action is to have a command with just a verb and no direct
         object or indirect object.
      */
      virtual bool checkSyntax(const std::shared_ptr<trogdor::core::Command> &command);

      virtual void execute(trogdor::core::entity::Player *player,
      const std::shared_ptr<trogdor::core::Command> &command, trogdor::core::Game *game);
};


#endif

