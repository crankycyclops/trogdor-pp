#include <algorithm>
#include "../../include/event/triggers/deathdrop.h"

using namespace std;

namespace trogdor { namespace core { namespace event {


   void DeathDropEventTrigger::execute(EventArgumentList args) {

      Being *being = static_cast<Being *>(boost::get<Entity *>(args[1]));

      entity::ObjectList drops;

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
}}}

