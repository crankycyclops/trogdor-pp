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

   void Creature::buildWeaponCache() {

      ObjectSetCItPair objs = getInventoryObjects();

      for (ObjectSetCIt o = objs.begin; o != objs.end; o++) {

         if ((*o)->isWeapon()) {

            bool inserted = false;

            // insert weapon into the cache in sorted order
            for (ObjectListIt i = weaponCache.begin(); i != weaponCache.end(); i++) {
               if ((*o)->getDamage() >= (*i)->getDamage()) {
                  weaponCache.insert(i, *o);
                  inserted = true;
                  break;
               }
            }

            // item belongs at the very back
            if (!inserted) {
               weaponCache.push_back(*o);
            }
         }
      }
   }

   Object *Creature::selectWeapon() {

      Dice d;

      // calculate probability that Creature will use a weapon
      double p = getAttributeFactor("dexterity") * 0.7;
      p = p > 0.0 ? p + 0.3 : 0.0;

      // creature was able to use a weapon
      if (d.get() < p) {

         if (0 == weaponCache.size()) {
            buildWeaponCache();
         }

         // TODO: choose weapon with some probability
         return *weaponCache.begin();
      }

      // creature either didn't have a weapon or couldn't use one
      return 0;
   }
}}

