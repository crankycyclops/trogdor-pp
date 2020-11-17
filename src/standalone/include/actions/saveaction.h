#ifndef SAVEACTION_H
#define SAVEACTION_H


#include <memory>
#include <trogdor/actions/action.h>


/*
   The Save action serializes the game state and saves it to a file.
*/
class SaveAction: public trogdor::Action {

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
