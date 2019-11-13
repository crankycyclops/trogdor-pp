#ifndef EVENTARG_H
#define EVENTARG_H


#include <string>
#include <vector>
#include <variant>

#include "../entities/entity.h"
#include "../entities/place.h"
#include "../entities/thing.h"
#include "../entities/room.h"
#include "../entities/being.h"
#include "../entities/player.h"
#include "../entities/object.h"
#include "../entities/creature.h"

using namespace std;

namespace trogdor { namespace event {


   // Allows us to pass a variable number of unknown type arguments to an
   // EventTrigger via EventHandler::event().
   typedef std::variant<int, double, bool, string, Game *, entity::Entity *> EventArgument;
   typedef vector<EventArgument> EventArgumentList;

}}


#endif

