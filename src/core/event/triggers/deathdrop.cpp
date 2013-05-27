#include "../../include/event/triggers/deathdrop.h"

using namespace std;

namespace core { namespace event {


   void DeathDropEventTrigger::execute(EventArgumentList args) {

      Being *being = static_cast<Being *>(boost::get<Entity *>(args[1]));

      // TODO: should we only drop with some probability?
      // TODO: right now, we drop undroppable objects; should we?
      entity::ObjectSetCItPair invItems = being->getInventoryObjects();
      for (ObjectSetCIt i = invItems.begin; i != invItems.end; i++) {
         being->drop(*i, false, false);
      }

      continueExecutionFlag = true;
      allowActionFlag = true;
   }
}}

