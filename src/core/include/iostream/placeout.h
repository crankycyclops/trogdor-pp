#ifndef PLACEOUT_H
#define PLACEOUT_H


#include <memory>

#include "trogout.h"
#include "../entities/place.h"


using namespace std;
using namespace trogdor::core::entity;

namespace trogdor { namespace core {


   /*
      Special meta output stream that sends output to all Entities present in a
      Place.
   */
   class PlaceOut: public Trogout {

      private:

         Place *place;

      public:

         /*
            See include/iostream/trogout.h for details.
         */
         virtual void flush();

         /*
            Constructor for PlaceOut that leaves the Place uninitialized.  This
            is required in cases where the Place is instantiated after the
            output stream.  flush() will check for a null pointer and won't do
            anything in that case, so this is safe.
         */
         inline PlaceOut() {place = 0;}

         /*
            Ordinary constructor that takes as input the Place to output to.
         */
         inline PlaceOut(Place *p) {place = p;}

         /*
            Allows us to set the Place after construction.

            Input:
               Place *

            Output:
               (none)
         */
         inline void setPlace(Place *p) {place = p;}

         /*
            See include/iostream/trogout.h for details.
         */
         virtual std::unique_ptr<Trogout> clone();
   };
}}


#endif

