#include <memory>
#include <iostream>
#include <cstring>
#include <random>

#include <trogdor/vocabulary.h>
#include <trogdor/actions.h>
#include <trogdor/event/event.h>

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
         "Well, ?&*@! to you, too!",
         "Do you kiss your mother with that mouth?",
         "Classy.",
         "Swear much?"
      };

      static int arrSize = sizeof(responses) / sizeof (const char *);

      static std::random_device rd;
      static std::minstd_rand generator(rd());
      static std::uniform_int_distribution<unsigned> distribution(0, arrSize - 1);

      player->out("display") << responses[distribution(generator)] << std::endl;
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

      if (auto location = player->getLocation().lock()) {

         if (object.length() == 0) {
            location->observe(player->getShared(), true, true);
         }

         else {

            ThingList items;

            // consider matching inventory items, if there are any
            for (auto const &invObject: player->getInventoryObjectsByName(object)) {
                  items.push_front(invObject);
            };

            // also consider matching items in the room, if there are any
            for (auto const &roomThing: location->getThingsByName(object)) {
               items.push_front(roomThing);
            };

            if (0 == items.size()) {
               player->out("display") << "There is no " << object << " here!" << std::endl;
               return;
            }

            try {
               Thing *thing = Entity::clarifyEntity<ThingList, Thing *>(items, player);
               thing->observe(player->getShared(), true, true);
            }

            catch (const std::string &name) {
               player->out("display") << "There is no " << name << " here!" << std::endl;
            }
         }
      }
   }

/******************************************************************************/

   /*
      Methods for the Read action.
   */

   bool ReadAction::checkSyntax(const std::shared_ptr<Command> &command) {

      // we must have a direct object and only a direct object
      if (!command->getDirectObject().length() || command->getIndirectObject().length()) {
         return false;
      }

      return true;
   }

   void ReadAction::execute(Player *player, const std::shared_ptr<Command> &command, Game *game) {

      std::string thingName = command->getDirectObject();

      if (auto location = player->getLocation().lock()) {

         if (thingName.length() == 0) {
            location->observe(player->getShared(), true, true);
         }

         else {

            ThingList items;

            // consider matching inventory items, if there are any
            for (auto const &invObject: player->getInventoryObjectsByName(thingName)) {
               items.push_front(invObject);
            };

            // also consider matching items in the room, if there are any
            for (auto const &roomThing: location->getThingsByName(thingName)) {
               items.push_front(roomThing);
            };

            if (0 == items.size()) {
               player->out("display") << "There is no " << thingName << " here!" << std::endl;
               return;
            }

            try {

               Thing *thing = Entity::clarifyEntity<ThingList, Thing *>(items, player);
               std::string text = thing->getMeta("text");

               if (!game->event({
                  "beforeRead",
                  {player->getEventListener(), thing->getEventListener()},
                  {player, thing}
               })) {
                  return;
               }

               if (text.length()) {
                  player->out("display") << text << std::endl;
               }

               else {
                  player->out("display") << "There's nothing to read." << std::endl;
               }

               game->event({
                  "afterRead",
                  {player->getEventListener(), thing->getEventListener()},
                  {player, thing}
               });
            }

            catch (const std::string &name) {
               player->out("display") << "There is no " << name << " here!" << std::endl;
            }
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

      if (auto location = player->getLocation().lock()) {

         auto roomItems = location->getThingsByName(command->getDirectObject());

         if (roomItems.begin() == roomItems.end()) {
            player->out("display") << "There is no " << command->getDirectObject()
               << " here!" << std::endl;
            return;
         }

         try {

            Thing *thing = Entity::clarifyEntity<ThingList, Thing *>(roomItems, player);

            if (ENTITY_OBJECT != thing->getType()) {
               player->out("display") << "You can't take that!" << std::endl;
            }

            else {

               try {

                  player->take(static_cast<Object *>(thing)->getShared());

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

      auto invItems = player->getInventoryObjectsByName(command->getDirectObject());

      if (invItems.begin() == invItems.end()) {
         player->out("display") << "You don't have a " << command->getDirectObject()
            << "!" << std::endl;
         return;
      }

      try {

         Object *object = Entity::clarifyEntity<ObjectList, Object *>(invItems, player);

         try {

            player->drop(object->getShared());

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

      if (auto location = player->getLocation().lock()) {

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
         if (ENTITY_ROOM != location->getType()) {
            player->out("display") << "You can't go that way." << std::endl;
            // TODO: fire can't go that way event?
            return;
         }

         auto next = (std::static_pointer_cast<Room>(location))->getConnection(direction);

         if (nullptr == next) {
            player->out("display") << "You can't go that way." << std::endl;
            // TODO: fire can't go that way event?
            return;
         }

         std::string enterMessage = next->getMessage("enter" + direction);
         std::string goMessage = location->getMessage("go" + direction);

         if (goMessage.length() > 0) {
            player->out("display") << goMessage << std::endl << std::endl;
         }

         if (enterMessage.length() > 0) {
            player->out("display") << enterMessage << std::endl << std::endl;
         }

         player->gotoLocation(next->getShared());
      }
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

      // The player is self-terminating
      // TODO: you can say "attack myself with fish" even if you don't have a
      // fish in your inventory. While technically not a big deal in the case of
      // self-termination, I should probably fix this.
      if (0 == strToLower(command->getDirectObject()).compare("myself")) {

         static const char *suicideResponses[] = {
            "Done.",
            "If you insist...",
            "You know, they have hotlines for that sort of thing.",
            "Was life really so hard?",
            "I hope you left a note.",
            "You'll be missed."
         };

         static int arrSize = sizeof(suicideResponses) / sizeof (const char *);

         static std::random_device rd;
         static std::minstd_rand generator(rd());
         static std::uniform_int_distribution<unsigned> distribution(0, arrSize - 1);

         if (player->isAlive()) {

            player->out("display") << suicideResponses[distribution(generator)] << std::endl;
            player->die(true);
         }

         else {
            player->out("display") << "You're already dead." << std::endl;
         }

         return;
      }

      if (auto location = player->getLocation().lock()) {

         auto beings = location->getBeingsByName(command->getDirectObject());

         if (beings.begin() == beings.end()) {
            player->out("display") << "There is no " << command->getDirectObject()
               << " here!" << std::endl;
            return;
         }

         try {

            std::string weaponName = command->getIndirectObject();
            Object *weapon = 0;

            Being *defender = Entity::clarifyEntity<BeingList, Being *>(beings, player);

            if (weaponName.length() > 0) {

               auto items = player->getInventoryObjectsByName(weaponName);

               if (items.begin() == items.end()) {
                  player->out("display") << "You don't have a " << weaponName << "!" << std::endl;
                  return;
               }

               try {

                  weapon = Entity::clarifyEntity<ObjectList, Object *>(items, player);

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
}
