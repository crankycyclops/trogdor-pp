#include <iostream>
#include <cstring>
#include <cstdlib>

#include "include/actions.h"

using namespace std;
using namespace core::entity;

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
      Methods for the Cuss action.
   */

   bool CussAction::checkSyntax(Command *command) {

      return true;
   }


   void CussAction::execute(Player *player, Command *command, Game *game) {

      static const char *responses[] = {
         "Such language!",
         "You, sir, have a foul mouth!",
         "Well, ?&*@! to you too!",
         "Do you kiss your mother with that mouth?",
         "Classy."
      };

      static int arrSize = sizeof(responses) / sizeof (const char *);

      int i = (rand() % arrSize) - 1;

      if (i < 0) {
         i = 0;
      }

      srand(time(NULL));
      *game->trogout << responses[i] << endl;
   }

/******************************************************************************/

   /*
      Methods for the Inventory action.
   */

   bool InventoryAction::checkSyntax(Command *command) {

      // A valid quit command should only be one word, a verb
      if (command->getDirectObject().length() > 0 ||
      command->getIndirectObject().length() > 0) {
         return false;
      }

      return true;
   }


   void InventoryAction::execute(Player *player, Command *command, Game *game) {

      // percentage of available space used
      double totalPercent = 0.0;

      if (0 == player->getInventoryCount()) {
         *game->trogout << "You don't have anything!" << endl;
      }

      else {

         ObjectSetCItPair invItems = player->getInventoryObjects();

         *game->trogout << "Items in your inventory:" << endl << endl;

         for (ObjectSet::const_iterator i = invItems.begin;
         i != invItems.end; i++) {

            *game->trogout << (*i)->getTitle();

            if (player->getInventoryMaxWeight() > 0) {

               if ((*i)->getWeight() > 0) {
                  double percent = 100 * ((double)(*i)->getWeight() /
                     (double)player->getInventoryMaxWeight());
                  totalPercent += percent;
                  *game->trogout << " (" << percent << "%)";
               }

               else {
                  *game->trogout << " (weighs nothing)";
               }
            }

            *game->trogout << endl;
         }

         if (player->getInventoryMaxWeight() > 0) {
            cout << endl << "You are currently using " << totalPercent
               << "% of your inventory." << endl;
         }
      }
   }

/******************************************************************************/

   /*
      Methods for the Look action.
   */

   bool LookAction::checkSyntax(Command *command) {

      // we can only have one of either
      if (command->getDirectObject().length() > 0 &&
      command->getIndirectObject().length() > 0) {
         return false;
      }

      return true;
   }

   void LookAction::execute(Player *player, Command *command, Game *game) {

      string object = command->getDirectObject();

      if (object.length() == 0) {
         object = command->getIndirectObject();
      }

      if (object.length() == 0) {
         player->getLocation()->observe(player, true, true);
      }

      else {

         Place *location = player->getLocation();

         // Note: I can't use list.merge(), because ObjectList and ThingList are
         // of different types :'(
         ThingList items;

         // consider matching inventory items, if there are any
         ObjectListCItPair invItems = player->getInventoryObjectsByName(object);
         for (ObjectListCIt i = invItems.begin; i != invItems.end; i++) {
               items.push_front(*i);
         }

         // also consider matching items in the room, if there are any
         ThingListCItPair roomItems = location->getThingsByName(object);
         for (ThingListCIt i = roomItems.begin; i != roomItems.end; i++) {
            items.push_front(*i);
         }

         if (0 == items.size()) {
            *game->trogout << "There is no " << object << " here!" << endl;
            return;
         }

         ThingListCItPair itemsIt;
         itemsIt.begin = items.begin();
         itemsIt.end   = items.end();

         try {
            Thing *thing =
               Entity::clarifyEntity2<ThingListCItPair, ThingListCIt, Thing *>(itemsIt,
               game->trogin, game->trogout);
            thing->observe(player, true, true);
         }

         catch (string name) {
            *game->trogout << "There is no " << name << " here!" << endl;
         }
      }
   }

/******************************************************************************/

   /*
      Methods for the Take action.
   */

   bool TakeAction::checkSyntax(Command *command) {

      if (command->getIndirectObject().length() > 0 ||
      command->getDirectObject().length() == 0) {
         return false;
      }

      return true;
   }

   void TakeAction::execute(Player *player, Command *command, Game *game) {

      Place     *location  = player->getLocation();
      ThingListCItPair roomItems = location->getThingsByName(command->getDirectObject());

      if (roomItems.begin == roomItems.end) {
         *game->trogout << "There is no " << command->getDirectObject()
            << " here!" << endl;
         return;
      }

      try {

         Thing *thing =
            Entity::clarifyEntity2<ThingListCItPair, ThingListCIt, Thing *>(roomItems,
            game->trogin, game->trogout);

         if (ENTITY_OBJECT != thing->getType()) {
            *game->trogout << "You can't take that!" << endl;
         }

         else {

            try {

               player->take(static_cast<Object *>(thing));

               string message = thing->getMessage("take");
               if (message.length() > 0) {
                  *game->trogout << message << endl;
               }

               else {
                  *game->trogout << "You take the " << thing->getName() << "." << endl;
               }
            }

            catch (enum Being::takeError error) {

               // TODO: consider custom messages
               switch (error) {

                  case Being::TAKE_TOO_HEAVY:
                     *game->trogout << command->getDirectObject()
                        << " is too heavy.  Try dropping something first."
                        << endl;
                     break;

                  case Being::TAKE_UNTAKEABLE:
                     *game->trogout << "You can't take that!" << endl;
                     break;

                  default:
                     *game->trogerr << "Unknown error taking object.  "
                        << "This is a bug." << endl;
                     break;
               }
            }
         }
      }

      catch (string name) {
         *game->trogout << "There is no " << name << " here!" << endl;
      }
   }

/******************************************************************************/

   /*
      Methods for the Drop action.
   */

   bool DropAction::checkSyntax(Command *command) {

      if (command->getIndirectObject().length() > 0 ||
      command->getDirectObject().length() == 0) {
         return false;
      }

      return true;
   }

   // TODO: consider custom messages
   void DropAction::execute(Player *player, Command *command, Game *game) {

      Place      *location = player->getLocation();
      ObjectListCItPair invItems;

      invItems = player->getInventoryObjectsByName(command->getDirectObject());

      if (invItems.begin == invItems.end) {
         *game->trogout << "You don't have a " << command->getDirectObject()
            << "!" << endl;
         return;
      }

      try {

         Object *object =
            Entity::clarifyEntity2<ObjectListCItPair, ObjectListCIt, Object *>(invItems,
            game->trogin, game->trogout);

         try {
 
            player->drop(object);

            string message = object->getMessage("drop");
            if (message.length() > 0) {
               *game->trogout << message << endl;
            }

            else {
               *game->trogout << "You drop the " << object->getName() << "." << endl;
            }
         }

         catch (enum Being::dropError error) {

            switch (error) {

               case Being::DROP_UNDROPPABLE:
                  // TODO: add message for this (named undroppable)
                  *game->trogout << "You can't drop that!" << endl;
                  break;

               default:
                  *game->trogerr << "Unknown error dropping object.  This is a "
                     << "bug." << endl;
                  break;
            }
         }
      }

      catch (string name) {
         *game->trogout << "You don't have a " << name << "!" << endl;
      }
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


   // TODO: consider custom messages for transitions
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

      // only Rooms have connections to eachother
      if (ENTITY_ROOM != player->getLocation()->getType()) {
         *game->trogout << "You can't go that way." << endl;
         // TODO: fire can't go that way event?
         return;
      }

      Room *next = (dynamic_cast<Room *>(player->getLocation()))->getConnection(direction);

      if (0 == next) {
         *game->trogout << "You can't go that way." << endl;
         // TODO: fire can't go that way event?
         return;
      }
      
      string enterMessage = next->getMessage("enter" + direction);
      string goMessage = player->getLocation()->getMessage("go" + direction);

      if (goMessage.length() > 0) {
         *game->trogout << goMessage << endl << endl;
      }

      if (enterMessage.length() > 0) {
         *game->trogout << enterMessage << endl << endl;
      }

      player->gotoLocation(next);
   }

/******************************************************************************/

   /*
      Methods for the Attack action.
   */

   bool AttackAction::checkSyntax(Command *command) {

      string verb = command->getVerb();
      string dobj = command->getDirectObject();
      string iobj = command->getIndirectObject();

      if (dobj.length() == 0) {
         return false;
      }

      return true;
   }


   void AttackAction::execute(Player *player, Command *command, Game *game) {

      Place *location = player->getLocation();
      BeingListCItPair beings = location->getBeingsByName(command->getDirectObject());

      if (beings.begin == beings.end) {
         *game->trogout << "There is no " << command->getDirectObject()
            << " here!" << endl;
         return;
      }

      try {

         string weaponName = command->getIndirectObject();
         Object *weapon = 0;

         Being *defender =
            Entity::clarifyEntity2<BeingListCItPair, BeingListCIt, Being *>(beings,
            game->trogin, game->trogout);

         if (weaponName.length() > 0) {

            ObjectListCItPair items = player->getInventoryObjectsByName(weaponName);

            if (items.begin == items.end) {
               *game->trogout << "You don't have a " << weaponName << "!" << endl;
               return;
            }

            try {

               weapon =
                  Entity::clarifyEntity2<ObjectListCItPair, ObjectListCIt, Object *>(items,
                  game->trogin, game->trogout);

               // TODO: this check should be made inside Being (we'd have an
               // exception to catch)
               if (!weapon->isWeapon()) {
                  *game->trogout << "The " << weaponName << " isn't a weapon!" << endl;
                  return;
               }
            }

            catch (string name) {
               *game->trogout << "You don't have a " << weaponName << "!" << endl;
               return;
            }
         }

         player->attack(defender, weapon);
      }

      catch (string name) {
         *game->trogout << "There is no " << name << " here!" << endl;
      }
   }
}

