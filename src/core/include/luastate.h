#ifndef LUASTATE_H
#define LUASTATE_H


#include <string>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

using namespace std;


namespace core {

   class LuaState {

      private:

         // Lua state
         lua_State *L;

         // error message that resulted from the last operation
         string lastErrorMsg;

      public:

         /*
            Constructor for the ScriptState object.
         */
         inline LuaState() {

            lastErrorMsg = "";
            L = luaL_newstate();

            // load standard library
            luaL_openlibs(L);
         }

         /*
            Returns the last error message that occured during an operation.

            Input:
               (none)

            Output:
               Error message (string)
         */
         inline string getLastErrorMsg() const {return lastErrorMsg;}

         /*
            Loads a script from the specified file.  If there's an error,
            lastErrorMsg will be set and an exception with that same message
            will be thrown.

            Input:
               filename (string)

            Output:
               (none)
         */
         void loadScriptFromFile(string filename);

         /*
            Loads a script from the specified string.  If there's an error, the
            child-specific implementation is expected to set lastErrorMsg and
            throw an exception with that same message as the argument.

            Input:
               script body (string)

            Output:
               (none)
         */
         void loadScriptFromString(string script);

         /*
            Push function arguments onto the Lua state's stack.

            Input:
               Value of the appropriate type

            Output:
               (none)
         */
         inline void pushNilArgument() {lua_pushnil(L);}
         inline void pushArgument(bool arg) {lua_pushboolean(L, (int)arg);}
         inline void pushArgument(string arg) {lua_pushstring(L, arg.c_str());}
         inline void pushArgument(int arg) {lua_pushnumber(L, (lua_Number)arg);}
         inline void pushArgument(double arg) {lua_pushnumber(L, (lua_Number)arg);}

         /*
            Executes the specified function.  If there's an error, lastErrorMsg
            will be set and false will be returned.  Otherwise, true will be
            returned to signal a successful execution.

            Input:
               function (string)

            Output:
               True if execution was successful and false if not
         */
         bool execute(string function);
   };

}


#endif

