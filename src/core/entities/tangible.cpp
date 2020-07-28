#include <trogdor/entities/tangible.h>


namespace trogdor::entity {


   Tangible::Tangible(Game *g, std::string n, std::unique_ptr<Trogout> o,
   std::unique_ptr<Trogerr> e): Entity(g, n, std::move(o), std::move(e)) {

      types.push_back(ENTITY_TANGIBLE);
   }

   /***************************************************************************/

   Tangible::Tangible(const Tangible &t, std::string n): Entity(t, n) {}
}
