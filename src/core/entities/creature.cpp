#include "../include/game.h"
#include "../include/entities/creature.h"


using namespace std;
using namespace boost;

namespace core { namespace entity {


   LuaTable *Creature::getLuaTable() const {

      LuaTable *table = Being::getLuaTable();

      table->setField("allegiance", getAllegianceStr());
      table->setField("counterattack", counterAttack);

      LuaTable *autoAttackArr = new LuaTable();
      autoAttackArr->setField("enabled", autoAttack.enabled);
      autoAttackArr->setField("interval", autoAttack.interval);
      autoAttackArr->setField("repeat", autoAttack.repeat);

      table->setField("autoattack", *autoAttackArr);

      return table;
   }
}}

