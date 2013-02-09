#ifndef ACTIONS_H
#define ACTIONS_H


#include "action.h"

using namespace std;

namespace core {

   /*
      The Quit action stops the game, ordinarily returning control to the client.
   */
   class QuitAction: public Action {

      /*
         See documentation in action.h.  A valid syntax for the Quit action
         is to have a command with just a verb and no direct object or
         indirect object.
      */
      virtual bool checkSyntax(Command command);

      virtual void execute(Command command);
   }; 
}


#endif

