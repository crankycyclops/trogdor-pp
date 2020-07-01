#include <trogdor/entities/being.h>
#include <trogdor/event/triggers/deathdrop.h>

namespace trogdor::event {


   EventReturn DeathDropEventTrigger::operator()(Event e) {

      entity::ObjectList drops;
      entity::Being *being = static_cast<entity::Being *>(std::get<entity::Entity *>(e.getArguments()[1]));

      // TODO: should we only drop with some probability?
      // TODO: right now, we drop undroppable objects; should we?
      if (!being->isAlive()) {

         for (auto const &object: being->getInventoryObjects()) {
            drops.push_back(object.get());
         };

         for_each(drops.begin(), drops.end(), [&](entity::Object * const &object) {
            being->drop(object->getShared(), false);
         });
      }

      return {true, true};
   }
}
