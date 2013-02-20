#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H


#include <string>

#include "eventtrigger.h"


using namespace std;

namespace core { namespace event {


   class EventListener {

      private:

         // set to false if we wish to suppress the action that originally
         // triggered the event
         int allowActionFlag;

         // set to false if we wish to stop executing any remaining event
         // triggers
         int continueExecutionFlag;

      public:

         /*
            Constructor for the EventListener class.
         */
         inline EventListener() {

            allowActionFlag = true;
            continueExecutionFlag = true;
         }

         /*
            Returns the state of the allowAction and continueExecution flags.
         */
         inline bool allowAction() const {return allowActionFlag;}
         inline bool continueExecution() const {return continueExecutionFlag;}

         /*
            Executes all EventTriggers for a given event.  
         */
         void execute(const char *event);
   };
}}


#endif

