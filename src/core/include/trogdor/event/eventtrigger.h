#ifndef EVENTTRIGGER_H
#define EVENTTRIGGER_H


#include <mutex>
#include <optional>
#include <typeinfo>
#include <string_view>
#include <trogdor/event/event.h>
#include <trogdor/serial/serializable.h>


namespace trogdor::event {


   class EventTrigger {

      private:

         // Maps class names to type ids for all registered event triggers
         static std::unordered_map<std::string_view, std::type_info *> types;

         // Registers built-in event trigger types
   		static void initBuiltinTypes();

      public:

         /*
            Returns the type id for the given class name if it exists. If not,
            returns std::nullopt.

            Input:
               Class's name (const char *)

            Output:
               Class's type id (std::optional<std::type_info>)
         */
         static inline std::optional<std::type_info *> getType(const char *typeName) {

            if (!types.size()) {
               initBuiltinTypes();
            }

            return types.find(typeName) != types.end() ?
               std::optional<std::type_info *>(types.find(typeName)->second) : std::nullopt;
         }

         /*
            When a class has one or more virtual functions, it should also have
            a virtual destructor.
         */
         virtual ~EventTrigger() = 0;

         /*
            Returns the class name of an EventTrigger instance.

            Input:
               (none)

            Output:
               Class name (const char *)
         */
         virtual const char *getClassName() = 0;

         /*
            Returns the event trigger's type id.

            Input:
               (none)

            Output:
               Class's type id (std::optional<std::type_info>)
         */
         inline std::type_info * getType() {

            return *getType(getClassName());
         }

         /*
            Executes the EventTrigger.

            Input:
               The event that triggered this method call (Event &e)

            Output:
               A pair of flags that determine whether or not execution of event
               listeners an their associated triggers should continue and
               whether or not the action that triggered the event should be
               allowed to continue or be suppressed (EventReturn)
         */
         virtual EventReturn operator()(Event e) = 0;

         /*
            Returns a serialized version of the EventTrigger instance.

            Input:
               (none)

            Output:
               Serialized instance of EventTrigger (std::shared_ptr<serial::Serializable>)
         */
         virtual std::shared_ptr<serial::Serializable> serialize();
   };
}


#endif
