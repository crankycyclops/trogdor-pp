#ifndef RESPAWNEVENTTRIGGER_H
#define RESPAWNEVENTTRIGGER_H


#include "../eventtrigger.h"


using namespace std;

namespace core { namespace event {


   class RespawnEventTrigger: public EventTrigger {

      public:

         /*
            Triggers a Being's respawning.

            Input:
               list of arguments

            Output:
               (none)
         */
         virtual void execute(EventArgumentList args);
   };
}}


#endif

