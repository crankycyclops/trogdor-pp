#include "../../include/timer/jobs/autoattack.h"


namespace trogdor {


   void AutoAttackTimerJob::execute() {

      if (!aggressor->isAlive() || !defender->isAlive()) {
         setExecutions(0);
         return;
      }

      else if (!defender->isTagSet(Being::AttackableTag)) {
         setExecutions(0);
         return;
      }

      else if (aggressor->getLocation() != defender->getLocation()) {
         setExecutions(0);
         return;
      }

      aggressor->attack(defender, aggressor->selectWeapon());
   }
}
