#ifndef EVENTARG_H
#define EVENTARG_H

#include <string>
#include <vector>
#include <variant>

// Forward declarations
namespace trogdor {

   class Game;

   namespace entity {
      class Entity;
   }
}

namespace trogdor::event {


   typedef std::variant<
      int, double, bool, std::string, Game *, entity::Entity *
   > EventArgument;

   /**************************************************************************/

   /*
      When an event is executed, this is the type of value that gets returned.
   */
   struct EventReturn {

      /*
         True if the action that triggered the event should be allowed to occur
         and false if it should be suppressed (for example, you might want to
         prevent a player from taking an object if a certain event trigger fires
         a certain way.)
      */
      bool allowAction = true;

      /*
         True if subsequent event listeners and triggers should be suppressed
         (for example, you might have an event trigger that fires at the game
         level that overrules event listeners for individual entities)
      */
      bool continueExecution = true;
   };
}


#endif
