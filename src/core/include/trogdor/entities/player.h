#ifndef PLAYER_H
#define PLAYER_H


#include <memory>
#include <trogdor/entities/being.h>


namespace trogdor::entity {


   class Player: public Being {

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

         /*
            Returns a smart pointer representing a raw Player pointer. Be careful
            with this and only call it on Entities you know are managed by smart
            pointers. If, for example, you call this method on entities that are
            managed by Lua using new and delete, you're going to have a bad time.

            Input:
               (none)

            Output:
               std::shared_ptr<Player>
         */
         inline std::shared_ptr<Player> getShared() {

            return std::dynamic_pointer_cast<Player>(Entity::getShared());
         }
   };
}


#endif
