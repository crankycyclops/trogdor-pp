#include <algorithm>
#include "../../include/event/triggers/deathdrop.h"

using namespace std;

namespace trogdor { namespace event {


   void DeathDropEventTrigger::execute(EventArgumentList args) {

      entity::ObjectList drops;
      entity::Being *being = static_cast<entity::Being *>(std::get<entity::Entity *>(args[1]));

      // TODO: should we only drop with some probability?
      // TODO: right now, we drop undroppable objects; should we?
      entity::ObjectSetCItPair invItems = being->getInventoryObjects();
      for_each(invItems.begin, invItems.end, [&](entity::Object * const &object) {
         drops.push_back(object);
      });

      for_each(drops.begin(), drops.end(), [&](entity::Object * const &object) {
         being->drop(object, false);
      });

      continueExecutionFlag = true;
      allowActionFlag = true;
   }
}}

