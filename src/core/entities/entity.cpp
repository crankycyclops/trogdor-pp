#include "../include/entities/entity.h"

#include "../include/game.h"
#include "../include/eventlistener.h"


using namespace std;

namespace core { namespace entity {


   Entity::Entity(Game *g, string n) {

      // not sure if this will ever actually be used, but meh...
      type = ENTITY_UNDEFINED;

      game = g;
      name = n;

      L = new LuaState();
      triggers = new event::EventListener();
   }

   /***************************************************************************/

   void Entity::display(Being *observer) {

      if (!observedBy(observer)) {

         if (ENTITY_PLAYER == observer->getType()) {
            *game->trogout << getLongDescription() << endl;
         }

         observedByMap[observer] = true;
      }

      else {

         if (ENTITY_PLAYER == observer->getType()
         && getShortDescription().length() > 0) {
            *game->trogout << getShortDescription() << endl;
         }
      }
   }

   /***************************************************************************/

   void Entity::observe(Being *observer, bool triggerEvents) {

      EventArgumentList eventArgs;

      if (triggerEvents) {

         eventArgs.push_back(this);
         eventArgs.push_back(observer);

         game->addEventListener(triggers);
         game->addEventListener(observer->getEventListener());

         if (!game->event("beforeObserve", eventArgs)) {
            return;
         }
      }

      display(observer);
      observedByMap[observer] = true;

      if (triggerEvents) {
         game->event("afterObserve", eventArgs);
      }
   }

   /***************************************************************************/

   void Entity::glance(Being *observer, bool triggerEvents) {

      // TODO: trigger event, displayShort, etc.
      glancedByMap[observer] = true;
      *game->trogout << "Glance stub!" << endl;
   }
}}

