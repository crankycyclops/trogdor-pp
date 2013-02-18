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

   void LuaState::pushArgument(LuaTable arg) {

      LuaTable::StringTable strings = arg.getStrings();
      LuaTable::NumberTable numbers = arg.getNumbers();
      LuaTable::BoolTable bools = arg.getBools();
      LuaTable::LuaTableTable tables = arg.getTables();

      lua_newtable(L);

      if (strings.size() > 0) {

         for (LuaTable::StringTable::iterator i = strings.begin();
         i != strings.end(); i++) {
            lua_pushstring(L, i->second.c_str());
            lua_setfield(L, lua_gettop(L), i->first.c_str());
         }
      }

      if (numbers.size() > 0) {
         for (LuaTable::NumberTable::iterator i = numbers.begin();
         i != numbers.end(); i++) {
            lua_pushnumber(L, i->second);
            lua_setfield(L, lua_gettop(L), i->first.c_str());
         }
      }

      if (bools.size() > 0) {
         for (LuaTable::BoolTable::iterator i = bools.begin();
         i != bools.end(); i++) {
            lua_pushboolean(L, i->second);
            lua_setfield(L, lua_gettop(L), i->first.c_str());
         }
      }

      if (tables.size() > 0) {
         // TODO
      }
   }

   /***************************************************************************/

   void LuaState::call(string function) {

      lua_getglobal(L, function.c_str());

      // only get ready to execute function if it exists
      if (!lua_isfunction(L, lua_gettop(L))) {
         lastErrorMsg = "function '" + function + "' does not exist";
         throw lastErrorMsg;
      }
   }

   /***************************************************************************/

   void LuaState::execute(int nReturnVals) {

      if (lua_pcall(L, nArgs, nReturnVals, 0)) {
         lastErrorMsg = "Script error: ";
         lastErrorMsg += lua_tostring(L, -1);
         nArgs = 0;
         throw lastErrorMsg;
      }

      nArgs = 0;
   }
}

