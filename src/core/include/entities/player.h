#ifndef PLAYER_H
#define PLAYER_H


#include "being.h"


namespace core { namespace entity {

   class Player: public Being {

      private:

      protected:

      public:

         /*
            Constructor for creating a new Player.  Requires reference to the
            containing Game object and a name.
         */
         inline Player(Game *g, Trogout *o, string n): Being(g, o, n) {

            type = ENTITY_PLAYER;

            attackable = true;
         }

         /*
            Constructor for cloning an existing player.  Requires a unique name.
         */
         inline Player(const Player &p, Trogout *o, string n): Being(p, n) {

            title = n;

            // hackety hack
            delete outStream;
            outStream = o;
         }

         /*
            Returns a LuaTable object representing the Player.  Note that each
            child class should get the value of the parent class's version of
            this method and then fill that object in further with its own
            values.

            Input:
               (none)

            Output:
               LuaTable object
         */
         virtual LuaTable *getLuaTable() const;
   };
}}


#endif

