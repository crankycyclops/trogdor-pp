#ifndef LUAEVENTTRIGGER_H
#define LUAEVENTTRIGGER_H


#include <memory>

#include "../../lua/luastate.h"
#include "../../lua/api/entities/luaentity.h"

#include "../eventtrigger.h"


namespace trogdor { namespace event {


   class LuaEventTrigger: public EventTrigger {

      private:

         std::string function;         // name of the function to execute
         std::shared_ptr<LuaState> L;  // lua state in which we'll execute the function

      public:

         /*
            Constructor for the LuaEventTrigger class. Takes as input the name
            of the Lua function to execute and a LuaState object which should
            contain the function.
         */
         inline LuaEventTrigger(std::string newfunc, std::shared_ptr<LuaState> newL): EventTrigger() {

            function = newfunc;
            L = newL;
         }

         /*
            Executes the function specified by the controller. WARNING: this
            method will throw an exception if it fails to execute the given Lua
            function. Make sure to catch this!

            Input:
               list of arguments to pass to the Lua function

            Output:
               (none)
         */
         virtual void execute(EventArgumentList args);
   };
}}


#endif
