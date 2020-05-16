#include <memory>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <cstdlib>

#include <trogdor/vocabulary.h>
#include <trogdor/actions.h>

#include <trogdor/exception/beingexception.h>

using namespace trogdor::entity;

namespace trogdor {


   /*
      Methods for the Cuss action.
   */

   bool CussAction::checkSyntax(const std::shared_ptr<Command> &command) {

      return true;
   }


   void CussAction::execute(Player *player, const std::shared_ptr<Command> &command, Game *game) {

      static const char *responses[] = {
         "Such language!",
         "You, sir, have a foul mouth!",
         "Well, ?&*@! to you too!",
         "Do you kiss your mother with that mouth?",
         "Classy.",
         "Swear much?"
      };

      static int arrSize = sizeof(responses) / sizeof (const char *);

      int i = (rand() % arrSize) - 1;

      if (i < 0) {
         i = 0;
      }

      srand(time(NULL));
      player->out("display") << responses[i] << std::endl;
   }

/******************************************************************************/

   /*
      Methods for the Inventory action.
   */

   bool InventoryAction::checkSyntax(const std::shared_ptr<Command> &command) {

      // A valid inventory command should only be one word, a verb
      if (command->getDirectObject().length() > 0 ||
      command->getIndirectObject().length() > 0) {
         return false;
      }

      return true;
   }


   void InventoryAction::execute(Player *player, const std::shared_ptr<Command> &command, Game *game) {

      // percentage of available space used
      double totalPercent = 0.0;

      if (0 == player->getInventoryCount()) {
         player->out("display") << "You don't have anything!" << std::endl;
      }

      else {

         player->out("display") << "Items in your inventory:" << std::endl << std::endl;

         for (auto const &obj: player->getInventoryObjects()) {

            player->out("display") << obj->getTitle();

            if (player->getInventoryMaxWeight() > 0) {

               if (obj->getWeight() > 0) {
                  double percent = 100 * ((double)obj->getWeight() /
                     (double)player->getInventoryMaxWeight());
                  totalPercent += percent;
                  player->out("display") << " (" << percent << "%)";
               }

               else {
                  player->out("display") << " (weighs nothing)";
               }
            }

            player->out("display") << std::endl;
         };

         if (player->getInventoryMaxWeight() > 0) {
            player->out("display") << std::endl << "You are currently using "
               << totalPercent << "% of your inventory." << std::endl;
         }
      }
   }

/******************************************************************************/

   /*
      Methods for the Look action.
   */

   bool LookAction::checkSyntax(const std::shared_ptr<Command> &command) {

      // we can only have one of either
      if (command->getDirectObject().length() > 0 &&
      command->getIndirectObject().length() > 0) {
         return false;
      }

      return true;
   }

   void LookAction::execute(Player *player, const std::shared_ptr<Command> &command, Game *game) {

      std::string object = command->getDirectObject();

      if (object.length() == 0) {
         object = command->getIndirectObject();
      }

      if (object.length() == 0) {
         player->getLocation()->observe(player, true, true);
      }

      else {

         ThingList items;
         Place *location = player->getLocation();

         // consider matching inventory items, if there are any
         ObjectListCItPair invItems = player->getInventoryObjectsByName(object);
         for_each(invItems.begin, invItems.end, [&](Object * const &invObject) {
               items.push_front(invObject);
         });

         // also consider matching items in the room, if there are any
         ThingListCItPair roomItems = location->getThingsByName(object);
         for_each(roomItems.begin, roomItems.end, [&](Thing * const &roomThing) {
            items.push_front(roomThing);
         });

         if (0 == items.size()) {
            player->out("display") << "There is no " << object << " here!" << std::endl;
            return;
         }

         ThingListCItPair itemsIt;
         itemsIt.begin = items.begin();
         itemsIt.end   = items.end();

         try {
            Thing *thing =
               Entity::clarifyEntity<ThingListCItPair, ThingListCIt, Thing *>(itemsIt,
               player);
            thing->observe(player, true, true);
         }

         catch (const std::string &name) {
            player->out("display") << "There is no " << name << " here!" << std::endl;
         }
      }
   }

/******************************************************************************/

   /*
      Methods for the Take action.
   */

   bool TakeAction::checkSyntax(const std::shared_ptr<Command> &command) {

      if (command->getIndirectObject().length() > 0 ||
      command->getDirectObject().length() == 0) {
         return false;
      }

      return true;
   }

   void TakeAction::execute(Player *player, const std::shared_ptr<Command> &command, Game *game) {

      Place            *location = player->getLocation();
      ThingListCItPair roomItems = location->getThingsByName(command->getDirectObject());

      if (roomItems.begin == roomItems.end) {
         player->out("display") << "There is no " << command->getDirectObject()
            << " here!" << std::endl;
         return;
      }

      try {

         Thing *thing =
            Entity::clarifyEntity<ThingListCItPair, ThingListCIt, Thing *>(roomItems,
            player);

         if (ENTITY_OBJECT != thing->getType()) {
            player->out("display") << "You can't take that!" << std::endl;
         }

         else {

            try {

               player->take(static_cast<Object *>(thing));

               std::string message = thing->getMessage("take");
               if (message.length() > 0) {
                  player->out("display") << message << std::endl;
               }

               else {
                  player->out("display") << "You take the " << thing->getName()
                     << "." << std::endl;
               }
            }

            catch (const entity::BeingException &e) {

               // TODO: consider custom messages
               switch (e.getErrorCode()) {

                  case entity::BeingException::TAKE_TOO_HEAVY:
                     player->out("display") << command->getDirectObject()
                        << " is too heavy.  Try dropping something first."
                        << std::endl;
                     break;

                  case entity::BeingException::TAKE_UNTAKEABLE:
                     player->out("display") << "You can't take that!" << std::endl;
                     break;

                  default:
                     player->err() << "Unknown error taking object.  "
                        << "This is a bug." << std::endl;
                     break;
               }
            }
         }
      }

      catch (const std::string &name) {
         player->out("display") << "There is no " << name << " here!" << std::endl;
      }
   }

/******************************************************************************/

   /*
      Methods for the Drop action.
   */

   bool DropAction::checkSyntax(const std::shared_ptr<Command> &command) {

      if (command->getIndirectObject().length() > 0 ||
      command->getDirectObject().length() == 0) {
         return false;
      }

      return true;
   }

   // TODO: consider custom messages
   void DropAction::execute(Player *player, const std::shared_ptr<Command> &command, Game *game) {

      ObjectListCItPair invItems;

      invItems = player->getInventoryObjectsByName(command->getDirectObject());

      if (invItems.begin == invItems.end) {
         player->out("display") << "You don't have a " << command->getDirectObject()
            << "!" << std::endl;
         return;
      }

      try {

         Object *object =
            Entity::clarifyEntity<ObjectListCItPair, ObjectListCIt, Object *>(invItems,
            player);

         try {

            player->drop(object);

            std::string message = object->getMessage("drop");
            if (message.length() > 0) {
               player->out("display") << message << std::endl;
            }

            else {
               player->out("display") << "You drop the " << object->getName()
                  << "." << std::endl;
            }
         }

         catch (const entity::BeingException &e) {

            switch (e.getErrorCode()) {

               case entity::BeingException::DROP_UNDROPPABLE:
                  // TODO: add message for this (named undroppable)
                  player->out("display") << "You can't drop that!" << std::endl;
                  break;

               default:
                  player->err() << "Unknown error dropping object.  This is a "
                     << "bug." << std::endl;
                  break;
            }
         }
      }

      catch (const std::string &name) {
         player->out("display") << "You don't have a " << name << "!" << std::endl;
      }
   }

/******************************************************************************/

   /*
      Methods for the Move action.
   */

   bool MoveAction::checkSyntax(const std::shared_ptr<Command> &command) {

      auto &vocab = command->getVocabulary();

      std::string verb = command->getVerb();
      std::string dobj = command->getDirectObject();
      std::string iobj = command->getIndirectObject();

      // A valid move command should not contain both a direct and indirect object
      if (dobj.length() > 0 && iobj.length() > 0) {
         return false;
      }

      // no direct or indirect object were given, so the direction, if valid,
      // must've been specified directly by the "verb"
      std::string dir = ((0 == dobj.length() && 0 == iobj.length()) ? verb :
         (dobj.length() > 0 ? dobj : iobj));
      return (vocab.isDirection(dir) || vocab.isDirectionSynonym(dir));
   }


   void MoveAction::execute(Player *player, const std::shared_ptr<Command> &command, Game *game) {

      auto &vocab = command->getVocabulary();
      std::string direction = "";

      // direction is implied in the verb
      if (vocab.isDirection(command->getVerb()) || vocab.isDirectionSynonym(command->getVerb())) {
         direction = vocab.getDirection(command->getVerb());
      }

      // direction was supplied as the direct or indirect object of another
      // verb like "move" or "go"
      if (0 == direction.length()) {
         direction = vocab.getDirection(command->getDirectObject().length() > 0 ?
            command->getDirectObject() : command->getIndirectObject());
      }

      // only Rooms have connections to eachother
      if (ENTITY_ROOM != player->getLocation()->getType()) {
         player->out("display") << "You can't go that way." << std::endl;
         // TODO: fire can't go that way event?
         return;
      }

      Room *next = (dynamic_cast<Room *>(player->getLocation()))->getConnection(direction);

      if (nullptr == next) {
         player->out("display") << "You can't go that way." << std::endl;
         // TODO: fire can't go that way event?
         return;
      }

      std::string enterMessage = next->getMessage("enter" + direction);
      std::string goMessage = player->getLocation()->getMessage("go" + direction);

      if (goMessage.length() > 0) {
         player->out("display") << goMessage << std::endl << std::endl;
      }

      if (enterMessage.length() > 0) {
         player->out("display") << enterMessage << std::endl << std::endl;
      }

      player->gotoLocation(next);
   }

/******************************************************************************/

   /*
      Methods for the Attack action.
   */

   bool AttackAction::checkSyntax(const std::shared_ptr<Command> &command) {

      std::string verb = command->getVerb();
      std::string dobj = command->getDirectObject();
      std::string iobj = command->getIndirectObject();

      if (dobj.length() == 0) {
         return false;
      }

      return true;
   }


   void AttackAction::execute(Player *player, const std::shared_ptr<Command> &command, Game *game) {

      Place            *location = player->getLocation();
      BeingListCItPair beings = location->getBeingsByName(command->getDirectObject());

      if (beings.begin == beings.end) {
         player->out("display") << "There is no " << command->getDirectObject()
            << " here!" << std::endl;
         return;
      }

      try {

         std::string weaponName = command->getIndirectObject();
         Object *weapon = 0;

         Being *defender =
            Entity::clarifyEntity<BeingListCItPair, BeingListCIt, Being *>(beings,
            player);

         if (weaponName.length() > 0) {

            ObjectListCItPair items = player->getInventoryObjectsByName(weaponName);

            if (items.begin == items.end) {
               player->out("display") << "You don't have a " << weaponName << "!" << std::endl;
               return;
            }

            try {

               weapon =
                  Entity::clarifyEntity<ObjectListCItPair, ObjectListCIt, Object *>(items,
                  player);

               // TODO: this check should be made inside Being (we'd have an
               // exception to catch)
               if (!weapon->isTagSet(Object::WeaponTag)) {
                  player->out("display") << "The " << weaponName << " isn't a weapon!" << std::endl;
                  return;
               }
            }

            catch (const std::string &name) {
               player->out("display") << "You don't have a " << weaponName << "!" << std::endl;
               return;
            }
         }

         player->attack(defender, weapon);
      }

      catch (const std::string &name) {
         player->out("display") << "There is no " << name << " here!" << std::endl;
      }
   }
}
