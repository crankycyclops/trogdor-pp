#ifndef LUAEVENTTRIGGER_H
#define LUAEVENTTRIGGER_H


#include <memory>

#include <trogdor/lua/luastate.h>
#include <trogdor/lua/api/entities/luaentity.h>
#include <trogdor/iostream/trogerr.h>

#include <trogdor/event/eventtrigger.h>


namespace trogdor::event {


   class LuaEventTrigger: public EventTrigger {

      private:

         std::string function;         // name of the function to execute
         std::shared_ptr<LuaState> L;  // lua state in which we'll execute the function

      public:

         // The event trigger's name. Used for type comparison.
         static constexpr const char *CLASS_NAME = "LuaEventTrigger";

         /*
            Registers the event trigger type so that EventTrigger knows how to
            copy and deserialize it later.

            Input:
               (none)

            Output:
               (none)
         */
         static void init();

         // Default Constructor
         inline LuaEventTrigger(
            std::string newfunc,
            const std::shared_ptr<LuaState> &newL
         ): EventTrigger(), function(newfunc), L(newL) {}

         // Copy Constructor
         LuaEventTrigger(const LuaEventTrigger &t) = default;

         // Deserialization Constructor. Takes as input an error stream, the
         // name of the Lua function to execute, and a LuaState object which
         // should contain the function.
         LuaEventTrigger(
            const serial::Serializable &data,
            const std::shared_ptr<LuaState> &newL
         );

         /*
            Returns the class's name.

            Input:
               (none)

            Output:
               Class name (const char *)
         */
         virtual const char *getClassName();

         /*
            Sets a new Lua state for the event trigger. This is necessary when
            an instance of LuaEventTrigger is copied for a new entity and needs
            to be assigned that new entity's Lua state.

            Input:
               New state (std::shared_ptr<LuaState>)

            Output:
               (none)
         */
         inline void setLuaState(std::shared_ptr<LuaState> newL) {L = newL;}

         /*
            Executes the function specified by the controller. WARNING: this
            method will throw an exception if it fails to execute the given Lua
            function. Make sure to catch this!

            Input:
               The event that triggered this method call (Event &e)

            Output:
               A pair of flags that determine whether or not execution of event
               listeners an their associated triggers should continue and
               whether or not the action that triggered the event should be
               allowed to continue or be suppressed (EventReturn)
         */
         virtual EventReturn operator()(Event e);

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
