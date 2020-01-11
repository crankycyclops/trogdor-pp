#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H


#include <string>
#include <unordered_map>

#include <trogdor/event/eventtrigger.h>
#include <trogdor/event/triggers/luaeventtrigger.h>


namespace trogdor { namespace event {


   class EventListener {

      public:

         typedef std::list<EventTrigger *> EventTriggerList;
         typedef std::unordered_map<std::string, EventTriggerList *> EventTriggersMap;

      private:

         // set to false if we wish to suppress the action that originally
         // triggered the event
         bool allowActionFlag;

         // set to false if we wish to stop executing any remaining event
         // triggers
         bool continueExecutionFlag;

         // Hash table mapping event name => list of EventTriggers
         EventTriggersMap triggers;

      public:

         /*
            Constructor for the EventListener class.
         */
         inline EventListener() {

            reset();
         }

         /*
            Destructor for the EventListener class.
         */
         inline ~EventListener() {

            // TODO: rewrite these for loops as range-based?
            for (EventTriggersMap::iterator i = triggers.begin();
            i != triggers.end(); i++) {

               for (EventTriggerList::iterator j = i->second->begin();
               j != i->second->end(); j++) {
                  delete *j;
               }

               delete i->second;
            }
         }

         inline void reset() {

            allowActionFlag = true;
            continueExecutionFlag = true;
         }

         /*
            Returns the state of the allowAction and continueExecution flags.
         */
         inline bool allowAction() const {return allowActionFlag;}
         inline bool continueExecution() const {return continueExecutionFlag;}

         /*
            Adds an EventTrigger to the list of things to execute when the
            specified event is triggered.

            Input:
               event name (string)
               Pointer to EventTrigger-derived object

            Output:
               (none)
         */
         void add(std::string event, EventTrigger *trigger);

         /*
            Executes all EventTriggers for a given event.  

            Input:
               event name (string)
               arguments (EventArgumentList)

            Output:
               (none)
         */
         void execute(std::string event, EventArgumentList args);
   };
}}


#endif
