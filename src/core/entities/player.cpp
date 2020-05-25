#include <trogdor/game.h>
#include <trogdor/entities/player.h>


namespace trogdor::entity {

   Player::Player(Game *g, std::string n, std::unique_ptr<Trogout> o,
   std::unique_ptr<Trogin> i, std::unique_ptr<Trogerr> e): Being(g, n,
   std::move(o), std::move(i), std::move(e)) {

      types.push_back(ENTITY_PLAYER);
      setClass("player");
   }

   /**************************************************************************/

   Player::Player(const Player &p, std::string n, std::unique_ptr<Trogout> o,
   std::unique_ptr<Trogin> i, std::unique_ptr<Trogerr> e): Being(p, n) {

      title = n;

      outStream = std::move(o);
      inStream = std::move(i);
      errStream = std::move(e);
   }
}
