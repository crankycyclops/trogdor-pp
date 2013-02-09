#ifndef ACTIONMAP_H
#define ACTIONMAP_H


#include <string>
#include <boost/unordered_map.hpp>

#include "game.h"
#include "action.h"

using namespace std;
using namespace boost;

namespace core {

   class ActionMap {

      private:

         // reference to the game in which the action map resides
         Game *game;

         // maps verbs to Action objects
         unordered_map<string, Action *> actionTable;

      public:

         /*
            Constructor for the ActionMap class.  Requires a reference to the
            game object that contains the ActionMap.
         */
         inline ActionMap(Game *gameRef) {game = gameRef;}

         inline void setAction(string verb, Action *action) {
            actionTable[verb] = action;
         }

         inline void removeAction(string verb) {actionTable.erase(verb);}

         Action *getAction(string verb) const;
   };
}


#endif

