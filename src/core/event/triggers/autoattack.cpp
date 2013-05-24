#include "../../include/event/triggers/autoattack.h"

using namespace std;

namespace core { namespace event {


   void AutoAttackEventTrigger::execute(EventArgumentList args) {

      // TODO

      continueExecutionFlag = true;
      allowActionFlag = true;
   }
}}

