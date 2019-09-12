#ifndef PLAYER_H
#define PLAYER_H


#include <memory>
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
         inline Player(Game *g, string n, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogin> i, std::unique_ptr<Trogout> e): Being(g, n,
         std::move(o), std::move(i), std::move(e)) {

            types.push_back(ENTITY_PLAYER);
            attackable = true;

            setClass("player");
         }

         /*
            Constructor for cloning an existing player.  Requires a unique name.
         */
         inline Player(const Player &p, string n, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogin> i, std::unique_ptr<Trogout> e): Being(p, n) {

            title = n;

            outStream = std::move(o);
            inStream = std::move(i);
            errStream = std::move(e);
         }
   };
}}}


#endif

