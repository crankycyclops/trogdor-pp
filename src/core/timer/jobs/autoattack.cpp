#include <trogdor/entities/creature.h>
#include <trogdor/timer/jobs/autoattack.h>


namespace trogdor {


   const char *AutoAttackTimerJob::getClassName() {

      return CLASS_NAME;
   }

   /**************************************************************************/

   AutoAttackTimerJob::AutoAttackTimerJob(const serial::Serializable &data, Game *g): TimerJob(data, g) {

      aggressor = g->getCreature(std::get<std::string>(*data.get("aggressor"))).get();
      defender = g->getBeing(std::get<std::string>(*data.get("defender"))).get();
   }

   /**************************************************************************/

   void AutoAttackTimerJob::init() {

      registerType(
         CLASS_NAME,
         const_cast<std::type_info *>(&typeid(AutoAttackTimerJob)),
         [] (std::any arg) -> std::shared_ptr<TimerJob> {

            // Invoke the deserialization constructor
            if (typeid(std::tuple<serial::Serializable, Game *>) == arg.type()) {
               auto args = std::any_cast<std::tuple<serial::Serializable, Game *> &>(arg);
               return std::make_shared<AutoAttackTimerJob>(std::get<0>(args), std::get<1>(args));
            }

            else {
               throw UndefinedException("Unsupported argument type in AutoAttackTimerJob instantiator");
            }
         }
      );
   }

   /**************************************************************************/

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

      data->set("aggressor", aggressor->getName());
      data->set("defender", defender->getName());

      return data;
   }
}
