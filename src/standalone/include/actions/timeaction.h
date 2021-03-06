#ifndef TIMEACTION_H
#define TIMEACTION_H


#include <memory>
#include <trogdor/actions/action.h>


/*
   TimeAction outputs the current game time to whichever player issues
   the command.
*/
class TimeAction: public trogdor::Action {

   public:

      /*
         See documentation in core/include/action.h.  A valid syntax for the
         Time action is to have a command with just a verb and no direct
         object or indirect object.
      */
      virtual bool checkSyntax(const trogdor::Command &command);

      virtual void execute(
         trogdor::entity::Player *player,
         const trogdor::Command &command,
         trogdor::Game *game
      );
};


#endif
