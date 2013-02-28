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

      public:

         /*
            Constructor for creating a new Thing.  Requires reference to the
            containing Game object and a name.
         */
         inline Thing(Game *g, string n): Entity(g, n) {

            // Name is also an alias that we can reference a Thing by
            aliases.push_back(n);
         }

         /*
            Returns list of Thing's aliases.

            Input:
               (none)

            Output:
               vector<string>
         */
         inline vector<string> const getAliases() const {return aliases;}

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

