#include "../include/iostream/placeout.h"
#include "../include/entities/thing.h"


using namespace std;
using namespace core::entity;

namespace core {


   void PlaceOut::flush() {

      if (0 != place) {

         ThingListCItPair things = place->getThings();

         for (ThingListCIt i = things.begin; i != things.end; i++) {
            **i << getBufferStr() << endl;
         }
      }
   }

   Trogout *PlaceOut::clone() {

      return new PlaceOut;
   }
}

