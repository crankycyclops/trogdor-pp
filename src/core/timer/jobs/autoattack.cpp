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

   /**************************************************************************/

   std::shared_ptr<serial::Serializable> AutoAttackTimerJob::serialize() {

      std::shared_ptr<serial::Serializable> data = std::make_shared<serial::Serializable>() = TimerJob::serialize();

      data->set("type", "AutoAttackTimerJob");
      data->set("aggressor", aggressor->getName());
      data->set("defender", defender->getName());

      return data;
   }
}
