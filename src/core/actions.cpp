#include <iostream>
#include <cstring>

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
         ObjectList *invItems = player->getInventoryObjectsByName(object);
         if (0 != invItems) {
            for (ObjectList::iterator i = invItems->begin(); i != invItems->end();
            i++) {
               items.push_front(*i);
            }
         }

         // also consider matching items in the room, if there are any
         ThingList *roomItems = location->getThingsByName(object);
         if (0 != roomItems) {
            for (ThingList::iterator i = roomItems->begin(); i != roomItems->end();
            i++) {
               items.push_front(*i);
            }
         }

         if (0 == items.size()) {
            *game->trogout << "There is no " << object << " here!" << endl;
            return;
         }

         try {
            Thing *thing = Entity::clarifyEntity<ThingList, Thing *>(items,
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
      ThingList *roomItems = location->getThingsByName(command->getDirectObject());

      if (0 == roomItems || 0 == roomItems->size()) {
         *game->trogout << "There is no " << command->getDirectObject()
            << " here!" << endl;
         return;
      }

      try {

         Thing *thing = Entity::clarifyEntity<ThingList, Thing *>(*roomItems,
            game->trogin, game->trogout);

         if (ENTITY_OBJECT != thing->getType()) {
            *game->trogout << "You can't take that!" << endl;
         }

         else {
            player->take(static_cast<Object *>(thing));
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

   void DropAction::execute(Player *player, Command *command, Game *game) {

      // TODO
      cout << "DROP ACTION STUB!" << endl;
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

      player->gotoLocation(next);
   }
}

