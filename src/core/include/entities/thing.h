#ifndef THING_H
#define THING_H


#include <vector>
#include <memory>

#include "entity.h"
#include "place.h"


namespace trogdor { namespace entity {


   class Thing: public Entity {

      protected:

         Place          *location;  // where the Thing is located
         vector<string>  aliases;   // list of aliases

         /*
            Overrides Entity::display() and shows a Thing's description in the
            proper format.

            Input:
               Being observing the Thing
               Whether or not to always display the long description

            Output:
               (none)
         */
         virtual void display(Being *observer, bool displayFull = false);

         /*
            Overrides Entity::displayShort() and shows a Thing's short
            description in the proper format.

            Input:
               Being observing the Thing

            Output:
               (none)
         */
         virtual void displayShort(Being *observer);

      public:

         /*
            Constructor for creating a new Thing.  Requires reference to the
            containing Game object and a name.
         */
         inline Thing(Game *g, string n, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogin> i, std::unique_ptr<Trogout> e): Entity(g, n,
         std::move(o), std::move(i), std::move(e)), location(nullptr) {

            types.push_back(ENTITY_THING);

            // Name is also an alias that we can reference a Thing by
            aliases.push_back(n);
         }

         /*
            Constructor for cloning a Thing.  Requires a unique name.
         */
         inline Thing(const Thing &t, string n): Entity(t, n) {

				location = t.location;

            // copy over existing aliases, minus the original Thing's name, and
            // then add the new name to our list of aliases
            aliases = t.aliases;
            aliases.erase(find(aliases.begin(), aliases.end(), t.name));
            aliases.push_back(n);
         }

         /*
            Returns the Thing's current location in the game.  If the Thing
            hasn't been placed anywhere, 0 will be returned.

            Input:
               (none)

            Output:
               Place *
         */
         inline Place *getLocation() const {return location;}

         /*
            Returns list of Thing's aliases.

            Input:
               (none)

            Output:
               vector<string>
         */
         inline vector<string> const getAliases() const {return aliases;}

         /*
            Set's the Thing's current location in the game.

            Input:
               Pointer to Place

            Output:
               (none)
         */
         inline void setLocation(Place *l) {location = l;}
         inline void setLocation(std::shared_ptr<Place> l) {setLocation(l.get());}

         /*
            Adds an alias to the Thing, which is another name that the Thing can
            be called.

            Input:
               New alias (string)

            Output:
               (none)
         */
         virtual void addAlias(string alias);

         // TODO: virtual void removeAlias(string alias);
   };
}}


#endif

