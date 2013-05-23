#ifndef PLAYER_H
#define PLAYER_H


#include "being.h"


namespace core { namespace entity {

   class Player: public Being {

      private:

      protected:

      public:

         /*
            Constructor for creating a new Player.  Requires reference to the
            containing Game object and a name.
         */
         inline Player(Game *g, string n): Being(g, n) {

            type = ENTITY_PLAYER;

            attackable = true;
         }

         /*
            Constructor for cloning an existing player.  Requires a unique name.
         */
         inline Player(const Player &p, string n): Being(p, n) {

            title = n;
         }
   };
}}


#endif

