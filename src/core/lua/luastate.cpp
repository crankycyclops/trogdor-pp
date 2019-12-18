#include <fstream>
#include <string>
#include <sstream>

#include "../include/game.h"
#include "../include/lua/luastate.h"
#include "../include/entities/entity.h"

#include "../include/exception/luaexception.h"
#include "../include/exception/undefinedexception.h"

namespace trogdor {


   void LuaState::registerGlobalGame() {

      LuaGame::registerLuaType(L);

      Game **gameLocation = (Game **)lua_newuserdata(L, sizeof(Game *));
      *gameLocation = game;

      luaL_getmetatable(L, LuaGame::MetatableName);
      lua_setmetatable(L, -2);

      // Scripts can access the Game object through the global variable "game"
      lua_setglobal(L, LuaGame::globalName);
   }

   /***************************************************************************/

   void LuaState::pushLuaValue(LuaValue v, lua_State *L) {

      switch (v.type) {

         case LUA_TYPE_STRING:
            lua_pushstring(L, std::get<std::string>(v.value).c_str());
            break;

         case LUA_TYPE_NUMBER:
            lua_pushnumber(L, std::get<double>(v.value));
            break;

         case LUA_TYPE_BOOLEAN:
            lua_pushboolean(L, std::get<bool>(v.value));
            break;

         case LUA_TYPE_ARRAY:
            pushArray(L, *std::get<LuaArray *>(v.value));
            break;

         case LUA_TYPE_TABLE:
            pushTable(L, *std::get<LuaTable *>(v.value));
            break;

         case LUA_TYPE_FUNCTION:
            throw UndefinedException("TODO: add support for function type");
            break;

         default:
            throw UndefinedException("LuaState::pushTable: invalid value type");
      }
   }

   /***************************************************************************/

   void LuaState::pushEntity(lua_State *L, entity::Entity *e) {

      if (nullptr == e) {
         lua_pushnil(L);
      }

      else {

         entity::Entity **eLocation = (entity::Entity **)lua_newuserdata(L, sizeof(entity::Entity *));
         *eLocation = e;

         switch (e->getType()) {

            case entity::ENTITY_ROOM:
               luaL_getmetatable(L, entity::LuaRoom::MetatableName);
               break;

            case entity::ENTITY_OBJECT:
               luaL_getmetatable(L, entity::LuaObject::MetatableName);
               break;

            case entity::ENTITY_CREATURE:
               luaL_getmetatable(L, entity::LuaCreature::MetatableName);
               break;

            case entity::ENTITY_PLAYER:
               luaL_getmetatable(L, entity::LuaPlayer::MetatableName);
               break;

            default:
               luaL_getmetatable(L, entity::LuaEntity::MetatableName);
               break;
         }

         // set the argument's type
         lua_setmetatable(L, -2);
      }
   }

   /***************************************************************************/

   void LuaState::pushArray(lua_State *L, LuaArray &arg) {

      lua_newtable(L);

      for (unsigned int i = 0; i < arg.size(); i++) {
         LuaValue v = arg[i];
         pushLuaValue(v, L);
         lua_rawseti(L, -2, i + 1);
      }
   }

   /***************************************************************************/

   void LuaState::pushTable(lua_State *L, LuaTable &arg) {

      LuaTable::TableValues values = arg.getValues();

      lua_newtable(L);

      for (LuaTable::TableValues::iterator i = values.begin();
      i != values.end(); i++) {

         std::string key = i->first;
         LuaValue v = i->second;

         pushLuaValue(v, L);

         lua_setfield(L, -2, key.c_str());
      }
   }

   /***************************************************************************/

   void LuaState::pushArgument(entity::Entity *e) {

      pushEntity(L, e);
      nArgs++;
   }

   /***************************************************************************/

   void LuaState::loadScriptFromFile(std::string filename) {

      int status;

      if ((status = luaL_loadfile(L, filename.c_str()))) {

         switch (status) {

            case LUA_ERRFILE:
               lastErrorMsg = "lua error: could not open " + filename;
               throw LuaException(lastErrorMsg);
               break;

            case LUA_ERRSYNTAX:
               lastErrorMsg = filename + ": " + lua_tostring(L, -1);
               throw LuaException(lastErrorMsg);
               break;

            case LUA_ERRMEM:
               lastErrorMsg = "lua error: out of memory";
               throw LuaException(lastErrorMsg);
               break;

            default:
               break;
         }
      }

      // remember the contents of this script so we can copy it to another state
      std::ifstream file(filename.c_str());
      std::stringstream fcontents;

      fcontents << file.rdbuf();
      parsedScriptData += fcontents.str();
      parsedScriptData += "\n";

      prime();
   }

   /***************************************************************************/

   void LuaState::loadScriptFromString(std::string script) {

      int status;

      if ((status = luaL_loadstring(L, script.c_str()))) {

         switch (status) {

            case LUA_ERRSYNTAX:
               lastErrorMsg = "lua error: ";
               lastErrorMsg += lua_tostring(L, -1);
               throw LuaException(lastErrorMsg);
               break;

            case LUA_ERRMEM:
               lastErrorMsg = "lua error: out of memory";
               throw LuaException(lastErrorMsg);
               break;

            default:
               break;
         }
      }

      // remember the contents of this script so we can copy it to another state
      parsedScriptData += script;
      parsedScriptData += "\n";

      prime();
   }

   /***************************************************************************/

   void LuaState::call(std::string function) {

      lua_getglobal(L, function.c_str());

      // only get ready to execute function if it exists
      if (!lua_isfunction(L, lua_gettop(L))) {
         lastErrorMsg = "function '" + function + "' does not exist";
         throw LuaException(lastErrorMsg);
      }
   }

   /***************************************************************************/

   void LuaState::execute(int nReturnVals) {

      if (lua_pcall(L, nArgs, nReturnVals, 0)) {
         lastErrorMsg = "script error: ";
         lastErrorMsg += lua_tostring(L, -1);
         nArgs = 0;
         throw LuaException(lastErrorMsg);
      }

      nArgs = 0;
      nReturnValues = nReturnVals;
   }
}

