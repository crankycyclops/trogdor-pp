#include "../../include/event/triggers/deathdrop.h"

using namespace std;

namespace core { namespace event {


   void DeathDropEventTrigger::execute(EventArgumentList args) {

      Being *being = static_cast<Being *>(boost::get<Entity *>(args[1]));

      entity::ObjectList drops;

      // TODO: should we only drop with some probability?
      // TODO: right now, we drop undroppable objects; should we?
      entity::ObjectSetCItPair invItems = being->getInventoryObjects();
      for (ObjectSetCIt i = invItems.begin; i != invItems.end; i++) {
         drops.push_back(*i);
      }

      for (entity::ObjectListCIt i = drops.begin(); i != drops.end(); i++) {
         being->drop(*i, false);
      }

      continueExecutionFlag = true;
      allowActionFlag = true;
   }
}}

