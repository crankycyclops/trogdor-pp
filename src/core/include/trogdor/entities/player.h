#ifndef PLAYER_H
#define PLAYER_H


#include <memory>
#include <trogdor/entities/being.h>


namespace trogdor::entity {


   class Player: public Being {

      private:

      protected:

      public:

         /*
            Constructor for creating a new Player.  Requires reference to the
            containing Game object and a name.
         */
         inline Player(Game *g, std::string n, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogin> i, std::unique_ptr<Trogerr> e): Being(g, n,
         std::move(o), std::move(i), std::move(e)) {

            types.push_back(ENTITY_PLAYER);
            setClass("player");
         }

         /*
            Constructor for cloning an existing player.  Requires a unique name.
         */
         inline Player(const Player &p, std::string n, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogin> i, std::unique_ptr<Trogerr> e): Being(p, n) {

            title = n;

            outStream = std::move(o);
            inStream = std::move(i);
            errStream = std::move(e);
         }
   };
}


#endif
