#ifndef ENTITY_TYPE_H
#define ENTITY_TYPE_H

namespace trogdor::entity {


   // used for run-time check of an Entity's type
   enum EntityType {
      ENTITY_UNDEFINED = 0,
      ENTITY_ENTITY = 1,
      ENTITY_PLACE = 2,
      ENTITY_ROOM = 3,
      ENTITY_THING = 4,
      ENTITY_OBJECT = 5,
      ENTITY_BEING = 6,
      ENTITY_PLAYER = 7,
      ENTITY_CREATURE = 8,
      ENTITY_TANGIBLE = 9,
      ENTITY_RESOURCE = 10
   };
};

#endif