#include "../include/game.h"
#include "../include/entities/item.h"


using namespace std;
using namespace boost;

namespace core { namespace entity {


   LuaTable *Item::getLuaTable() const {

      LuaTable *table = Thing::getLuaTable();

      table->setField("takeable", takeable);
      table->setField("droppable", droppable);
      table->setField("isweapon", weapon);
      table->setField("damage", damage);

      if (0 == owner) {
         table->setField("owner", false);
      }

      else {
         table->setField("owner", owner->getName());
      }

      return table;
   }
}}

