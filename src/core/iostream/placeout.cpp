#include <algorithm>

#include <trogdor/iostream/placeout.h>
#include <trogdor/entities/thing.h>


using namespace trogdor::entity;

namespace trogdor {


   void PlaceOut::flush() {

      if (0 != place) {

         for (auto const &thing: place->getThings()) {
            thing->out(getChannel()) << getBufferStr() << std::endl;
         };
      }
   }

   std::unique_ptr<Trogout> PlaceOut::clone() {

      return std::make_unique<PlaceOut>();
   }
}
