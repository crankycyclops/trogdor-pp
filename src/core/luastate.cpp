#include <string>
#include "include/luastate.h"

using namespace std;

namespace core {


   void LuaState::loadScriptFromFile(string filename) {

      int status;

      if (status = luaL_loadfile(L, filename.c_str())) {

         switch (status) {

            case LUA_ERRFILE:
               throw "lua error: could not open " + filename;
               break;

            case LUA_ERRSYNTAX:
               throw filename + ": " + lua_tostring(L, -1);
               break;

            case LUA_ERRMEM:
               throw "lua error: out of memory";
               break;

            default:
               break;
         }
      }
   }

   /***************************************************************************/

   void LuaState::loadScriptFromString(string script) {

      // TODO
   }

   /***************************************************************************/

   bool LuaState::execute(string function) {

      // TODO
   }
}

