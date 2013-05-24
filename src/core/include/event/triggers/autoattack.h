#ifndef AUTOATTACKEVENTTRIGGER_H
#define AUTOATTACKEVENTTRIGGER_H


#include "../eventtrigger.h"


using namespace std;

namespace core { namespace event {


   class AutoAttackEventTrigger: public EventTrigger {

      public:

         /*
            Scans the room entered by a Being for hostile creatures and
            initiates auto-attack for any creatures that have that feature
            enabled.

            Input:
               list of arguments

            Output:
               (none)
         */
         virtual void execute(EventArgumentList args);
   };
}}


#endif

