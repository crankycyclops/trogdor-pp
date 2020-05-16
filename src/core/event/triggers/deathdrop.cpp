#include <trogdor/event/triggers/deathdrop.h>

namespace trogdor::event {


   void DeathDropEventTrigger::execute(EventArgumentList args) {

      entity::ObjectList drops;
      entity::Being *being = static_cast<entity::Being *>(std::get<entity::Entity *>(args[1]));

      // TODO: should we only drop with some probability?
      // TODO: right now, we drop undroppable objects; should we?
      for (auto const &object: being->getInventoryObjects()) {
         drops.push_back(object);
      };

      for_each(drops.begin(), drops.end(), [&](entity::Object * const &object) {
         being->drop(object, false);
      });

      continueExecutionFlag = true;
      allowActionFlag = true;
   }
}
