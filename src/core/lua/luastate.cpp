#include <string>

#include "../include/lua/luastate.h"
#include "../include/entities/entity.h"

using namespace std;

namespace core {


   void LuaState::pushTable(LuaTable &arg) {

      LuaTable::TableValues values = arg.getValues();

      lua_newtable(L);

      for (LuaTable::TableValues::iterator i = values.begin();
      i != values.end(); i++) {

         string key = i->first;
         LuaTableValue v = i->second;

         switch (v.type) {

            case LuaTableValue::LUATABLE_VALUE_STRING:
               lua_pushstring(L, boost::get<std::string>(v.value).c_str());
               break;

            case LuaTableValue::LUATABLE_VALUE_NUMBER:
               lua_pushnumber(L, boost::get<double>(v.value));
               break;

            case LuaTableValue::LUATABLE_VALUE_BOOLEAN:
               lua_pushboolean(L, boost::get<bool>(v.value));
               break;

            case LuaTableValue::LUATABLE_VALUE_TABLE:
               pushTable(*boost::get<LuaTable *>(v.value));
               break;

            // TODO: add support for function type

            default:
               throw "LuaState::pushTable: invalid type: wtf happened...?";
         }

         lua_setfield(L, -2, key.c_str());
      }
   }

   /***************************************************************************/

   void LuaState::pushArgument(entity::Entity *e) {

      entity::Entity **eLocation = (entity::Entity **)lua_newuserdata(L, sizeof(entity::Entity *));
      *eLocation = e;

      switch (e->getType()) {

         case entity::ENTITY_ROOM:
            luaL_getmetatable(L, "Room");
            break;

         case entity::ENTITY_OBJECT:
            luaL_getmetatable(L, "Object");
            break;

         case entity::ENTITY_CREATURE:
            luaL_getmetatable(L, "Creature");
            break;

         case entity::ENTITY_PLAYER:
            luaL_getmetatable(L, "Player");
            break;

         default:
            luaL_getmetatable(L, "Entity");
            break;
      }

      // set the argument's type
      lua_setmetatable(L, -2);

      nArgs++;
   }

   /***************************************************************************/

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
         lastErrorMsg = "script error: ";
         lastErrorMsg += lua_tostring(L, -1);
         nArgs = 0;
         throw lastErrorMsg;
      }

      nArgs = 0;
      nReturnValues = nReturnVals;
   }
}
