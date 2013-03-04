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

   void Entity::observe(Being *observer, bool triggerEvents) {

      // TODO
      observedByMap[observer] = true;
      *game->trogout << "Observe stub!" << endl;
   }

   /***************************************************************************/

   void Entity::glance(Being *observer, bool triggerEvents) {

      // TODO
      glancedByMap[observer] = true;
      *game->trogout << "Glance stub!" << endl;
   }
}}

