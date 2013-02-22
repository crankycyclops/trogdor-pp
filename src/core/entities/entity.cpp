#include "../include/entities/entity.h"

#include "../include/game.h"
#include "../include/eventlistener.h"


using namespace std;

namespace core { namespace entity {


   Entity::Entity(Game *g, string n) {

      game = g;
      name = n;

      L = new LuaState();
      triggers = new event::EventListener();
   }
}}

