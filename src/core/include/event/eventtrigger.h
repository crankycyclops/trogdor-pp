#ifndef EVENTTRIGGER_H
#define EVENTTRIGGER_H


#include "eventarg.h"


namespace trogdor { namespace event {


   class EventTrigger {

      protected:

         // set to false if we wish to suppress the action that originally
         // triggered the event
         bool allowActionFlag;

         // set to false if we wish to stop executing any remaining event
         // triggers
         bool continueExecutionFlag;

      public:

         /*
            Constructor for the EventTrigger class.
         */
         inline EventTrigger() {

            allowActionFlag = true;
            continueExecutionFlag = true;
         }

         /*
            When a class has one or more virtual functions, it should also have
            a virtual destructor.
         */
         virtual ~EventTrigger() = 0;

         /*
            Returns the state of the allowAction and continueExecution flags.
         */
         inline bool allowAction() const {return allowActionFlag;}
         inline bool continueExecution() const {return continueExecutionFlag;}

         /*
            Executes the EventTrigger.  Sets the two flags, allowActionFlag,
            which signals whether or not the action that triggered the event
            should be allowed to do its thing, and continueExecutionFlag, which
            determines whether or not we should continue executing EventTriggers
            for the event when ours is done.

            Input:
               EventArgumentList (variable number of variable type arguments)

            Output:
               (none)
         */
         virtual void execute(EventArgumentList args) = 0;
   };
}}


#endif
