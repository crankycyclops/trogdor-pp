#ifndef CUSS_ACTION_H
#define CUSS_ACTION_H


#include <trogdor/actions/action.h>


namespace trogdor {

   // The Cuss action responds to players when they cuss ;)
   class CussAction: public Action {

      public:

         virtual bool checkSyntax(const Command &command);

         virtual void execute(
            entity::Player *player,
            const Command &command,
            Game *game
         );
   };
}


#endif
