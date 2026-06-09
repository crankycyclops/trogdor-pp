#include <trogdor/entities/creature.h>
#include <trogdor/timer/jobs/autoattack.h>


namespace trogdor {


   const char *AutoAttackTimerJob::getClassName() {

      return CLASS_NAME;
   }

   /**************************************************************************/

   AutoAttackTimerJob::AutoAttackTimerJob(const serial::Serializable &data, Game *g): TimerJob(data, g) {

      aggressor = g->getCreature(std::get<std::string>(*data.get("aggressor")));
      defender = g->getBeing(std::get<std::string>(*data.get("defender")));
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

      std::shared_ptr<entity::Creature> attacker = aggressor.lock();
      std::shared_ptr<entity::Being> target = defender.lock();

      // Make sure one or both entities haven't been removed from the game
      if (!attacker || !target) {
         setExecutions(0);
         return;
      }

      if (!attacker->isAlive() || !target->isAlive()) {
         setExecutions(0);
         return;
      }

      else if (!target->isTagSet(entity::Being::AttackableTag)) {
         setExecutions(0);
         return;
      }

      else if (attacker->getLocation().lock() != target->getLocation().lock()) {
         setExecutions(0);
         return;
      }

      attacker->attack(target.get(), attacker->selectWeapon());
   }

   /**************************************************************************/

   std::shared_ptr<serial::Serializable> AutoAttackTimerJob::serialize() {

      std::shared_ptr<serial::Serializable> data = std::make_shared<serial::Serializable>() = TimerJob::serialize();

      if (auto attacker = aggressor.lock()) {
         data->set("aggressor", attacker->getName());
      }

      if (auto target = defender.lock()) {
         data->set("defender", target->getName());
      }

      return data;
   }
}
