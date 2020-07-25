#ifndef QUITACTION_H
#define QUITACTION_H


#include <memory>
#include <trogdor/actions/action.h>


/*
   The Quit action removes the player who issued the command from the game.
*/
class QuitAction: public trogdor::Action {

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
