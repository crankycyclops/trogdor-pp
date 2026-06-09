#include <memory>

#include <trogdor/entities/being.h>

#include <trogdor/event/triggers/respawn.h>
#include <trogdor/timer/jobs/respawn.h>

namespace trogdor::event {


   RespawnEventTrigger::RespawnEventTrigger(const serial::Serializable &data) {}

   /**************************************************************************/

   const char *RespawnEventTrigger::getClassName() {

      return CLASS_NAME;
   }

   /**************************************************************************/

   void RespawnEventTrigger::init() {

      registerType(
         CLASS_NAME,
         const_cast<std::type_info *>(&typeid(RespawnEventTrigger)),
         [] (std::any arg) -> std::unique_ptr<EventTrigger> {

            // Invoke the copy constructor
            if (typeid(RespawnEventTrigger *) == arg.type()) {
               return std::make_unique<RespawnEventTrigger>(*std::any_cast<RespawnEventTrigger *>(arg));
            }

            // Invoke the deserialization constructor
            else if (typeid(serial::Serializable) == arg.type()) {
               return std::make_unique<RespawnEventTrigger>(std::any_cast<serial::Serializable &>(arg));
            }

            else {
               throw UndefinedException("Unsupported argument type in RespawnEventTrigger instantiator");
            }
         }
      );
   }

   /**************************************************************************/

   EventReturn RespawnEventTrigger::operator()(Event e) {

      Game  *game  = std::get<Game *>(e.getArguments()[0]);
      entity::Being *being = static_cast<entity::Being *>(std::get<entity::Entity *>(e.getArguments()[1]));

      if (being->getProperty<bool>(entity::Being::RespawnEnabledProperty)) {

         int in = being->getProperty<int>(entity::Being::RespawnIntervalProperty);

         // we have to wait a certain number of clock ticks
         if (in > 0) {

            // The timer job needs to extract a std::weak_ptr from the Being
            // in case it goes out of scope between now and when it executes,
            // so pass in a std::shared_ptr.
            std::shared_ptr<entity::Being> deadGuy =
               std::static_pointer_cast<entity::Being>(being->getShared());

            game->insertTimerJob(
               std::make_shared<RespawnTimerJob>(game, in, 1, in, deadGuy)
            );
         }

         // respawn right away
         else {
            being->doRespawn();
         }
      }

      return {true, true};
   }

   /**************************************************************************/

   std::shared_ptr<serial::Serializable> RespawnEventTrigger::serialize() {

      return EventTrigger::serialize();
   }
}
