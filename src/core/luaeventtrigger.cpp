#include "include/luaeventtrigger.h"

using namespace std;

namespace core { namespace event {


   void LuaEventTrigger::execute(EventArgumentList args) {

      L->call(function);
      // TODO: parse and push args onto the Lua stack
      L->execute();

      // TODO: get return values (have to implement this in LuaState first) and
      // set allowActionFlag and continueHandlersFlag.
   }
}}

