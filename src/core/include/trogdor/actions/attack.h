#ifndef ATTACK_ACTION_H
#define ATTACK_ACTION_H


#include <trogdor/actions/action.h>


namespace trogdor {


   // The Attack action allows a player to attack another Being.
   class AttackAction: public Action {

      public:

         /*
            See documentation in action.h.  A direct object is required.  An
            indirect object is optional, and would specify a weapon to use in
            the attack.
         */
         virtual bool checkSyntax(const Command &command);

         virtual void execute(
            entity::Player *player,
            const Command &command,
            Game *game
         );
   };
}


#endif
