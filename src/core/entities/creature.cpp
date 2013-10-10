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

   Object *Creature::selectWeapon() {

      Dice d;

      // calculate probability that Creature will use a weapon
      double p = getAttributeFactor("dexterity") * 0.7;
      p = p > 0.0 ? p + 0.3 : 0.0;

      // creature was able to use a weapon
      if (d.get() < p) {

         // TODO: order weapons according to strength, in descending
         // order, then loop through them and select next strongest
         // weapon with some probability determined by intelligence

         ObjectSetCItPair objs = getInventoryObjects();

         // for now, just grab the first weapon we can find...
         for (ObjectSetCIt o = objs.begin; o != objs.end; o++) {
            if ((*o)->isWeapon()) {
               return *o;
            }
         }
      }

      // creature either didn't have a weapon or couldn't use one
      return 0;
   }
}}

