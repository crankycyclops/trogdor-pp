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
         Player(Game *g, std::string n, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogin> i, std::unique_ptr<Trogerr> e);

         /*
            Constructor for cloning an existing player.  Requires a unique name.
         */
         Player(const Player &p, std::string n, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogin> i, std::unique_ptr<Trogerr> e);
   };
}


#endif
