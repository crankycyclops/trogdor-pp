#ifndef LUASTATE_H
#define LUASTATE_H


#include <string>

extern "C" {
   #include <lua.h>
   #include <lualib.h>
   #include <lauxlib.h>
}

#include "luatable.h"
#include "api/entities/luaentity.h"
#include "api/entities/luathing.h"


using namespace std;


namespace core {

   namespace entity {
      class Entity;
   }

   /*
      LuaState wraps around an (aptly named) lua state variable and allows us to
      perform basic operations on it, specifically loading scripts and calling
      functions.  A typical workflow might look something like this:

      LuaState L;

      try {
         L.loadScriptFromFile("script.lua");
         L.call("functionName");
         L.pushArgument("this is a string");
         L.execute();

         // examples of retrieving return values (0 based from first to last)
         // calling these before L.execute() results in undefined behavior
         double retVal1 = L.getNumber(0); // 1st return value
         string retVal2 = L.getString(1); // 2nd return value
      }

      // handle errors
      catch (string error) {
         cout << error << endl;
      }

   */
   class LuaState {

      private:

         // number of function arguments pushed onto the Lua stack
         int nArgs;

         // set everytime we run execute(), and used to retrieve return values
         int nReturnValues;

         // Lua state
         lua_State *L;

         // error message that resulted from the last operation
         string lastErrorMsg;

         /*
            Called by pushArgument(LuaTable arg) and by pushTable recursively
            (for nested tables) to do the actual work of pushing a table onto
            the Lua stack for passing to a function.

            Input:
               Reference to LuaTable object

            Output:
               (none)
         */
         void pushTable(LuaTable &arg);

      public:

         /*
            Like luaL_checkudata, except that it accepts an array of possible
            types instead of just one. Shamelessly stolen from:
            http://lua-users.org/lists/lua-l/2007-04/msg00324.html

            Input:
               Lua state
               index of user data on the stack
               array of C-style strings naming valid types
         */
         inline static void *luaL_checkudata_ex(lua_State *L, int ud, const char **tnames) {

            int i;
            void *p = lua_touserdata(L, ud);

            if (p != NULL) {                                         // value is a userdata?
               if (lua_getmetatable(L, ud)) {                        // does it have a metatable?
                  for (i = 0; tnames[i] != NULL; i++) {              // compare to each tname given
                     lua_getfield(L, LUA_REGISTRYINDEX, tnames[i]);  // get correct mt */
                     if (lua_rawequal(L, -1, -2)) {                  // does it have the correct mt?
                        lua_pop(L, 2);                               // remove both metatables
                        return p;
                     }
                  }
               }
            }

            // compose error message that mentions all typenames
            {
               /* separate scope to avoid default stack of > LUAL_BUFFERSIZE
                  assuming it even makes a difference ... */
               luaL_Buffer B;

               luaL_buffinit(L, &B);

               for (i = 0; tnames[i] != NULL; i++) {
                  luaL_addstring(&B, tnames[i]);
                     if (tnames[i+1] != NULL) luaL_addlstring(&B, " or ", 4);
               }

               luaL_pushresult(&B);
               luaL_typerror(L, ud, lua_tostring(L, -1));
               return NULL; /* to avoid warnings */
            }
         }

         /*
            Constructor for the LuaState object.
         */
         inline LuaState() {

            nArgs = 0;
            nReturnValues = 0;
            lastErrorMsg = "";
            L = luaL_newstate();

            // load standard library
            luaL_openlibs(L);

            // register global types
            entity::LuaEntity::registerLuaType(L);
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
         inline void pushNilArgument() {

            lua_pushnil(L);
            nArgs++;
         }

         inline void pushArgument(const char *arg) {

            lua_pushstring(L, arg);
            nArgs++;
         }

         inline void pushArgument(string arg) {

            lua_pushstring(L, arg.c_str());
            nArgs++;
         }

         inline void pushArgument(double arg) {

            lua_pushnumber(L, (lua_Number)arg);
            nArgs++;
         }

         inline void pushArgument(bool arg) {

            lua_pushboolean(L, (int)arg);
            nArgs++;
         }

         inline void pushArgument(LuaTable &arg) {

            nArgs++;
            pushTable(arg);
         }

         void pushArgument(entity::Entity *e);

         /*
            Get return values at the specified index.  Calling this before
            calling execute() results in undefined behavior.

            Input:
               index (normalized, so that 0 is the index of the first return
               value, 1 is the index of the next, and so on and so forth...)

            Output:
               value of the appropriate type
         */
         inline bool getBoolean(int i) const {

            return lua_toboolean(L, i - nReturnValues);
         }

         inline double getNumber(int i) const {

            return lua_tonumber(L, i - nReturnValues);
         }

         inline string getString(int i) const {

            return lua_tostring(L, i - nReturnValues);
         }

         /*
            Primes the Lua state by parsing it so that recently added scripts,
            global variables, etc. will be seen by the interpreter.  This gets
            called automatically by loadScriptFromFile() and
            loadScriptFromString(), but may also be called manually if desired.
            Throws an exception with a message (string) if there's an error.

            Input:
               (none)

            Output:
               (none)
         */
         inline void prime() {

            if (lua_pcall(L, 0, 0, 0)) {
               string s = "error: " + string(lua_tostring(L, -1));
               throw s;
            }
         }

         /*
            Gets us ready to execute the specified function.  This should always
            be called BEFORE running passArgument() and friends.  And exception
            with an error message will be thrown if the function doesn't exist
            or if there's an error.

            Input:
               function name (string)

            Output:
               (none)
         */
         void call(string function);

         /*
            Executes the function set up by LuaState::call().  If there's an
            error, lastErrorMsg will be set and an exception will be thrown.

            Input:
               number of return values (default is 0)

            Output:
               (none)
         */
         void execute(int nReturnVals = 0);
   };
}


#endif

