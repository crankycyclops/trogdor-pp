#ifndef TAKE_ACTION_H
#define TAKE_ACTION_H


#include <unordered_map>

#include <trogdor/entities/player.h>
#include <trogdor/actions/action.h>

#include <trogdor/exception/beingexception.h>


namespace trogdor {

   // Allows players to take objects in the game and add them to their
   // inventories.
   class TakeAction: public Action {

      private:

         // If a player is executing this action from an input interceptor that
         // attempts to pick one Thing to look at from a list of available
         // options with the same alias, make sure we lookup the item by its
         // name rather than its alias to avoid infinite calls to clarifyEntity()
         // in the case where the desired object's name matches an alias with
         // more than one Thing.
         std::unordered_map<
            entity::Player *,
            std::weak_ptr<entity::Player>
         > lookupThingByName;

         // Takes the object and adds it to the player's inventory
         inline void take(entity::Player *player, entity::Thing *thing) {

            if (entity::ENTITY_OBJECT != thing->getType()) {
               player->out("display") << "You can't take that!" << std::endl;
            }

            else {

               try {

                  player->take(static_cast<entity::Object *>(thing)->getShared());

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
                        player->out("display") << thing->getName()
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

      public:

         /*
            See documentation in action.h.  A valid syntax for the Take action
            is to have a verb + direct object.  The result is for the Object
            to be taken into the Being's inventory, if possible.
         */
         virtual bool checkSyntax(const Command &command);

         virtual void execute(
            entity::Player *player,
            const Command &command,
            Game *game
         ); 
   };
}


#endif
