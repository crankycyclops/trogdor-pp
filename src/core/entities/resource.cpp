#include <trogdor/entities/resource.h>

#include <trogdor/iostream/nullout.h>
#include <trogdor/iostream/nullerr.h>


namespace trogdor::entity {


   Resource::Resource(Game *g, std::string n):
   Entity(g, n, std::make_unique<NullOut>(), std::make_unique<NullErr>()) {

      types.push_back(ENTITY_RESOURCE);
   }

   /***************************************************************************/

   Resource::Resource(const Resource &t, std::string n): Entity(t, n) {}
}
