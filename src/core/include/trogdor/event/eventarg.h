#ifndef EVENTARG_H
#define EVENTARG_H


#include <string>
#include <vector>
#include <variant>

#include <trogdor/entities/entity.h>
#include <trogdor/entities/place.h>
#include <trogdor/entities/thing.h>
#include <trogdor/entities/room.h>
#include <trogdor/entities/being.h>
#include <trogdor/entities/player.h>
#include <trogdor/entities/object.h>
#include <trogdor/entities/creature.h>

namespace trogdor { namespace event {


   // Allows us to pass a variable number of unknown type arguments to an
   // EventTrigger via EventHandler::event().
   typedef std::variant<int, double, bool, std::string, Game *, entity::Entity *> EventArgument;
   typedef std::vector<EventArgument> EventArgumentList;

}}


#endif
