#ifndef LOADACTION_H
#define LOADACTION_H


#include <memory>
#include <trogdor/actions/action.h>


/*
   The Load action restores a previously serialized game state from a file.
*/
class LoadAction: public trogdor::Action {

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
