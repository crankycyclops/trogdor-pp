#include <memory>

#include <trogdor/entities/place.h>
#include <trogdor/entities/creature.h>

#include <trogdor/event/triggers/autoattack.h>
#include <trogdor/timer/jobs/autoattack.h>

namespace trogdor::event {


   AutoAttackEventTrigger::AutoAttackEventTrigger(const serial::Serializable &data) {

      // TODO
   }

   /**************************************************************************/

   const char *AutoAttackEventTrigger::getClassName() {

      return CLASS_NAME;
   }

   /**************************************************************************/

   void AutoAttackEventTrigger::init() {

      registerType(
         CLASS_NAME,
         const_cast<std::type_info *>(&typeid(AutoAttackEventTrigger)),
         [] (std::any arg) -> std::unique_ptr<EventTrigger> {

            // Invoke the copy constructor
            if (typeid(AutoAttackEventTrigger *) == arg.type()) {
               return std::make_unique<AutoAttackEventTrigger>(*std::any_cast<AutoAttackEventTrigger *>(arg));
            }

            // Invoke the deserialization constructor
            else if (typeid(serial::Serializable) == arg.type()) {
               return std::make_unique<AutoAttackEventTrigger>(std::any_cast<serial::Serializable &>(arg));
            }

            else {
               throw UndefinedException("Unsupported argument type in AutoAttackEventTrigger instantiator");
            }
         }
      );
   }

   /**************************************************************************/

   EventReturn AutoAttackEventTrigger::operator()(Event e) {

      Game  *game  = std::get<Game *>(e.getArguments()[0]);
      entity::Being *being = static_cast<entity::Being *>(std::get<entity::Entity *>(e.getArguments()[1]));
      entity::Place *place = static_cast<entity::Place *>(std::get<entity::Entity *>(e.getArguments()[3]));

      // each Creature that has auto-attack enabled should be setup to attack
      for (auto const &creature: place->getCreatures()) {

         if (creature->getProperty<bool>(entity::Creature::AutoAttackEnabledProperty)) {
            game->insertTimerJob(std::make_shared<AutoAttackTimerJob>(
               game,
               creature->getProperty<int>(entity::Creature::AutoAttackIntervalProperty),
               creature->getProperty<bool>(entity::Creature::AutoAttackRepeatProperty) ? -1 : 1,
               creature->getProperty<int>(entity::Creature::AutoAttackIntervalProperty),
               creature.get(),
               being
            ));
         }
      };

      return {true, true};
   }

   /**************************************************************************/

   std::shared_ptr<serial::Serializable> AutoAttackEventTrigger::serialize() {

      return EventTrigger::serialize();
   }
}
