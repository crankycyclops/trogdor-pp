#include <algorithm>

#include "../include/iostream/placeout.h"
#include "../include/entities/thing.h"


using namespace trogdor::entity;

namespace trogdor {


   void PlaceOut::flush() {

      if (0 != place) {

         ThingListCItPair things = place->getThings();

         for_each(things.begin, things.end, [&](Thing * const &thing) {
            thing->out(getChannel()) << getBufferStr() << endl;
         });
      }
   }

   std::unique_ptr<Trogout> PlaceOut::clone() {

      return std::make_unique<PlaceOut>();
   }
}

