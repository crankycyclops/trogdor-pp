#include <cstdio>
#include <algorithm>

#include <trogdor/entities/thing.h>
#include <trogdor/entities/being.h>
#include <trogdor/game.h>


namespace trogdor { namespace entity {


   void Thing::display(Being *observer, bool displayFull) {

      observer->out("display") << "You see " << getTitle() << '.' << std::endl;
      Entity::display(observer, displayFull);
   }

   /***************************************************************************/

   void Thing::displayShort(Being *observer) {

      observer->out("display") << "You see " << getTitle() << '.' << std::endl;
      Entity::displayShort(observer);
   }

   /***************************************************************************/

   void Thing::addAlias(std::string alias) {

      // The number of aliases should be small enough that it's not a big deal
      // to iterate through the vector, and an alias shouldn't appear in the
      // vector more than once (otherwise, subtle but nasty things happen.)
      if (std::find(aliases.begin(), aliases.end(), alias) == aliases.end()) {

         aliases.insert(aliases.end(), alias);

         // If we're adding the alias after the Thing is already at a location, we
         // need to re-insert it to update the Place's thingsByName map.
         if (location != nullptr) {
            Place *p = location; // removeThing sets location back to nullptr, so...
            p->removeThing(this);
            p->insertThing(this);
         }
      }
   }
}}
