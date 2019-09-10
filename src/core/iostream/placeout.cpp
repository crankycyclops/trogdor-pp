#include <algorithm>

#include "../include/iostream/placeout.h"
#include "../include/entities/thing.h"


using namespace std;
using namespace trogdor::core::entity;

namespace trogdor { namespace core {


   void PlaceOut::flush() {

      if (0 != place) {

         ThingListCItPair things = place->getThings();

         for_each(things.begin, things.end, [&](Thing * const &thing) {
            thing->out(getChannel()) << getBufferStr() << endl;
         });
      }
   }

   Trogout *PlaceOut::clone() {

      return new PlaceOut;
   }
}}

