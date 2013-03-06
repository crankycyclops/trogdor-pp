#ifndef THING_H
#define THING_H



#include <vector>
#include "entity.h"
#include "place.h"


namespace core { namespace entity {

   class Thing: public Entity {

      protected:

         Place          *location;  // where the Thing is located
         vector<string>  aliases;   // list of aliases

         /*
            Overrides Entity::display() and shows a Thing's description in the
            proper format.

            Input:
               Being observing the Thing

            Output:
               (none)
         */
         virtual void display(Being *observer);

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
         inline Thing(Game *g, string n): Entity(g, n) {

            location = 0;

            // Name is also an alias that we can reference a Thing by
            aliases.push_back(n);
         }

         /*
            Returns the Being's current location in the game.  If the Being
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
            Set's the Being's current location in the game.

            Input:
               Pointer to Place

            Output:
               (none)
         */
         inline void setLocation(Place *l) {location = l;}

         /*
            Adds an alias to the Thing, which is another name that the Thing can
            be called.

            Input:
               New alias (string)

            Output:
               (none)
         */
         inline void addAlias(string alias) {aliases.insert(aliases.end(), alias);}
   };
}}


#endif

