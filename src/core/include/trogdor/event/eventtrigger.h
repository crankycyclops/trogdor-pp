#ifndef EVENTTRIGGER_H
#define EVENTTRIGGER_H


#include <mutex>
#include <optional>
#include <functional>
#include <typeinfo>
#include <typeindex>
#include <string_view>
#include <trogdor/event/event.h>
#include <trogdor/serial/serializable.h>
#include <trogdor/exception/undefinedexception.h>


namespace trogdor::event {


   class EventTrigger {

      private:

         // Maps class names to type ids for all registered event triggers
         static std::unordered_map<std::string_view, std::type_info *> types;

         // Maps type ids to callbacks that can create instances of EventTrigger
         // that are the appropriate type. Depending on the data that's passed
         // in, callbacks should either handle copy construction or
         // deserialization.
         static std::unordered_map<
            std::type_index,
            std::function<std::unique_ptr<EventTrigger>(std::any)>
         > instantiators;

         // Registers built-in event trigger types
   		static void registerBuiltinTypes();

      protected:

         /*
            Registers a new event trigger type so that we know how to copy and
            deserialize it later.

            Input:
               Name of derived class (const char *)
               The class's type id (std::type_info *)
               A callback that can handle copy construction and deserialization

            Output:
               (none)
         */
         inline static void registerType(
            const char *name,
            std::type_info *type,
            std::function<std::unique_ptr<EventTrigger>(std::any)> instantiator
         ) {
            types[name] = type;
            instantiators[std::type_index(*type)] = instantiator;
         }

      public:

         /*
            Returns the type_info of the given trigger name.

            Input:
               Type name (const char *)
               Arguments to pass to instantiator callback (std::any)

            Output:
               const std::type_info &

            Throws an instance of UndefinedException if the type hasn't been
            registered.
         */
         inline static const std::type_info &getType(const char *name) {

            if (!types.size()) {
               registerBuiltinTypes();
            }

            if (auto type = types.find(name); type != types.end()) {
               return *type->second;
            } else {
               throw UndefinedException(
                  std::string("EventTrigger type '") + name + "' has not been registered"
               );
            }
         }

         /*
            Instantiates a copy constructed or deserialized child of EventTrigger.

            Input:
               Type name (const char *)
               Arguments to pass to instantiator callback (std::any)

            Output:
               Copied or deserialized instance of EventTrigger (std::unique_ptr<EventTrigger>)
         */
         inline static std::unique_ptr<EventTrigger> instantiate(
            const char *type,
            std::any args
         ) {

            if (!types.size()) {
               registerBuiltinTypes();
            }

            return instantiators[std::type_index(*types[type])](args);
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
