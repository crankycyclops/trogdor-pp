#include <cstdio>
#include <algorithm>

#include "../include/entities/thing.h"
#include "../include/entities/being.h"
#include "../include/game.h"


using namespace std;

namespace trogdor { namespace entity {


   void Thing::display(Being *observer, bool displayFull) {

      observer->out("display") << "You see " << getTitle() << '.' << endl;
      Entity::display(observer, displayFull);
   }

   /***************************************************************************/

   void Thing::displayShort(Being *observer) {

      observer->out("display") << "You see " << getTitle() << '.' << endl;
      Entity::displayShort(observer);
   }

   /***************************************************************************/

   void Thing::addAlias(string alias) {

      // The number of aliases should be small enough that it's not a big deal
      // to iterate through the vector, and only calling insert if the alias
      // doesn't already exist minimizes the potential for side effects (it also
      // fixes a weird bug I encountered.)
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

