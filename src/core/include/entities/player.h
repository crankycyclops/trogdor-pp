#ifndef PLAYER_H
#define PLAYER_H


#include "being.h"


namespace trogdor { namespace core { namespace entity {

   class Player: public Being {

      private:

      protected:

      public:

         /*
            Constructor for creating a new Player.  Requires reference to the
            containing Game object and a name.
         */
         inline Player(Game *g, string n, Trogout *o, Trogin *i, Trogout *e):
         Being(g, n, o, i, e) {

            types.push_back(ENTITY_PLAYER);
            attackable = true;

            setClass("player");
         }

         /*
            Constructor for cloning an existing player.  Requires a unique name.
         */
         inline Player(const Player &p, string n, Trogout *o, Trogin *i, Trogout *e):
         Being(p, n) {

            title = n;

            // hackety hack
            delete outStream;
            delete inStream;
            delete errStream;
            outStream = o;
            inStream = i;
            errStream = e;
         }
   };
}}}


#endif

