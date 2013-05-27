#include "../../include/event/triggers/deathdrop.h"

using namespace std;

namespace core { namespace event {


   void DeathDropEventTrigger::execute(EventArgumentList args) {

      Being *being = static_cast<Being *>(boost::get<Entity *>(args[1]));

      // TODO
      // entity::ObjectListCItPair invItems = being->;
      cout << "Death drop stub!" << endl;

      continueExecutionFlag = true;
      allowActionFlag = true;
   }
}}

