#include <trogdor/entities/creature.h>
#include <trogdor/timer/jobs/autoattack.h>


namespace trogdor {


   void AutoAttackTimerJob::execute() {

      if (!aggressor->isAlive() || !defender->isAlive()) {
         setExecutions(0);
         return;
      }

      else if (!defender->isTagSet(entity::Being::AttackableTag)) {
         setExecutions(0);
         return;
      }

      else if (aggressor->getLocation().lock() != defender->getLocation().lock()) {
         setExecutions(0);
         return;
      }

      aggressor->attack(defender, aggressor->selectWeapon());
   }
}
