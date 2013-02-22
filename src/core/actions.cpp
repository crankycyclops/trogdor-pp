#include <iostream>
#include <cstring>
#include "include/actions.h"

using namespace std;

namespace core {


   /*
      Methods for the Quit action.
   */

   bool QuitAction::checkSyntax(Command *command) {

      // A valid quit command should only be one word, a verb
      if (command->getDirectObject().length() > 0 ||
      command->getIndirectObject().length() > 0) {
         return false;
      }

      return true;
   }


   void QuitAction::execute(Player *player, Command *command, Game *game) {

      game->removePlayer(player->getName());
   }

/******************************************************************************/

   /*
      Methods for the Move action.
   */

   bool MoveAction::checkSyntax(Command *command) {

      string verb = command->getVerb();
      string dobj = command->getDirectObject();
      string iobj = command->getIndirectObject();

      // A valid quit command should not contain both a direct and indirect object
      if (dobj.length() > 0 && iobj.length() > 0) {
         return false;
      }

      // no direct or indirect object were given, so the direction, if valid,
      // must've been specified directly by the "verb"
      if (0 == dobj.length() && 0 == iobj.length()) {

         for (int i = 0; g_directions[i] != 0; i++) {
            if (0 == strcmp(verb.c_str(), g_directions[i])) {
               return true;
            }
         }

         return false;
      }

      // make sure the direction specified by the object was valid
      string direction = dobj.length() > 0 ? dobj : iobj;
      for (int i = 0; g_directions[i] != 0; i++) {
         if (0 == strcmp(direction.c_str(), g_directions[i])) {
            return true;
         }
      }

      return false;
   }


   void MoveAction::execute(Player *player, Command *command, Game *game) {

      string direction = "";

      // direction is implied in the verb
      for (int i = 0; g_directions[i] != 0; i++) {
         if (0 == strcmp(command->getVerb().c_str(), g_directions[i])) {
            direction = g_directions[i];
         }
      }

      // direction was supplied as the direct or indirect object of another
      // verb like "move" or "go"
      if (0 == direction.length()) {
         direction = command->getDirectObject().length() > 0 ?
            command->getDirectObject() : command->getIndirectObject();
      }

      // TODO
      cout << "Stub: going " << direction << "!" << endl;
   }
}

