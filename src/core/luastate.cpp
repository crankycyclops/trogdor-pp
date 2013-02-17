#include <string>
#include "include/luastate.h"

using namespace std;

namespace core {


   void LuaState::loadScriptFromFile(string filename) {

      int status;

      if (status = luaL_loadfile(L, filename.c_str())) {

         switch (status) {

            case LUA_ERRFILE:
               lastErrorMsg = "lua error: could not open " + filename;
               throw lastErrorMsg;
               break;

            case LUA_ERRSYNTAX:
               lastErrorMsg = filename + ": " + lua_tostring(L, -1);
               throw lastErrorMsg;
               break;

            case LUA_ERRMEM:
               lastErrorMsg = "lua error: out of memory";
               throw lastErrorMsg;
               break;

            default:
               break;
         }
      }

      prime();
   }

   /***************************************************************************/

   void LuaState::loadScriptFromString(string script) {

      int status;

      if (status = luaL_loadstring(L, script.c_str())) {

         switch (status) {

            case LUA_ERRSYNTAX:
               lastErrorMsg = "lua error: ";
               lastErrorMsg += lua_tostring(L, -1);
               throw lastErrorMsg;
               break;

            case LUA_ERRMEM:
               lastErrorMsg = "lua error: out of memory";
               throw lastErrorMsg;
               break;

            default:
               break;
         }
      }

      prime();
   }

   /***************************************************************************/

   bool LuaState::execute(string function, int nReturnVals) {

      lua_getglobal(L, function.c_str());

      // only call function if it exists
      if (!lua_isfunction(L, lua_gettop(L))) {
         lastErrorMsg = "function '" + function + "' does not exist";
         return false;
      }

      if (lua_pcall(L, nArgs, nReturnVals, 0)) {
         lastErrorMsg = "Script error: ";
         lastErrorMsg += lua_tostring(L, -1);
         return false;
      }
   }
}

