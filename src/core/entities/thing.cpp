#include <cstdio>
#include <algorithm>

#include <trogdor/entities/thing.h>
#include <trogdor/entities/being.h>
#include <trogdor/game.h>


namespace trogdor::entity {


   Thing::Thing(Game *g, std::string n, std::unique_ptr<Trogout> o,
   std::unique_ptr<Trogin> i, std::unique_ptr<Trogerr> e): Entity(g, n,
   std::move(o), std::move(i), std::move(e)), location(nullptr) {

      types.push_back(ENTITY_THING);

      // Name is also an alias that we can reference a Thing by
      aliases.push_back(n);
   }

   /***************************************************************************/

   Thing::Thing(const Thing &t, std::string n): Entity(t, n) {

      location = t.location;

      // copy over existing aliases, minus the original Thing's name, and
      // then add the new name to our list of aliases
      aliases = t.aliases;
      aliases.erase(find(aliases.begin(), aliases.end(), t.name));
      aliases.push_back(n);
   }

   /***************************************************************************/

   void Thing::display(Being *observer, bool displayFull) {

      observer->out("display") << "You see " << getTitle() << '.' << std::endl;
      Entity::display(observer, displayFull);
   }

   /***************************************************************************/

   void Thing::displayShort(Being *observer) {

      if (shortDesc.length()) {
         Entity::displayShort(observer);
      }

      else {
         observer->out("display") << "You see " << getTitle() << '.' << std::endl;
      }
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
}
