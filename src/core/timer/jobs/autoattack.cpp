#include "../../include/timer/jobs/autoattack.h"


using namespace std;

namespace core {


   void AutoAttackTimerJob::execute(TimerJob &job) {

      if (!aggressor->isAlive() || !defender->isAlive()) {
         job.setExecutions(0);
         return;
      }

      else if (!defender->isAttackable()) {
         job.setExecutions(0);
         return;
      }

      else if (aggressor->getLocation() != defender->getLocation()) {
         job.setExecutions(0);
         return;
      }

      aggressor->attack(defender, aggressor->selectWeapon());
   }
}

