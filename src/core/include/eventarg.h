#ifndef EVENTARG_H
#define EVENTARG_H


#include <string>
#include <vector>
#include <boost/variant.hpp>

#include "entities/entity.h"
#include "entities/place.h"
#include "entities/thing.h"
#include "entities/room.h"
#include "entities/being.h"
#include "entities/item.h"
#include "entities/player.h"
#include "entities/object.h"
#include "entities/creature.h"

using namespace std;
using namespace core::entity;

namespace core { namespace event {

   // Allows us to pass a variable number of unknown type arguments to an
   // EventTrigger via EventHandler::event().
   typedef boost::variant<int, double, bool, string, Entity *, Place *, Thing *,
      Room *, Being *, Item *, Player *, Creature *, Object *> EventArgument;
   typedef vector<EventArgument> EventArgumentList;

}}


#endif

