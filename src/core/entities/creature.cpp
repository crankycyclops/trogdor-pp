#include "../include/game.h"
#include "../include/entities/creature.h"


using namespace std;
using namespace boost;

namespace core { namespace entity {


   LuaTable *Creature::getLuaTable() const {

      LuaTable *table = Being::getLuaTable();

      table->setField("allegiance", getAllegianceStr());
      table->setField("counterattack", counterAttack);

      return table;
   }
}}

