#include "../../../include/game.h"

#include "../../../include/entities/entity.h"
#include "../../../include/entities/being.h"

#include "../../../include/lua/api/entities/luaentity.h"
#include "../../../include/lua/api/entities/luabeing.h"

using namespace std;

namespace trogdor { namespace entity {


   // The name of the metatable that represents the Entity metatable
   const char *LuaEntity::MetatableName = "Entity";

   // This is the name of the library that contains functions related to
   // Entities in the game
   const char *LuaEntity::PackageName = "Entity";

   // Types which are considered valid by checkEntity()
   static const char *entityTypes[] = {
      "Entity",
      "Place",
      "Room",
      "Thing",
      "Object",
      "Being",
      "Player",
      "Creature",
      0
   };

   // Non-object oriented functions that can't be called with the colon
   // operator or passed an instance of self as the first argument.
   static const luaL_Reg functions[] = {
      {"get", LuaEntity::getEntity},
      {0, 0}
   };

   // Lua Entity methods that bind to C++ Entity methods. These should be called
   // with the colon operator or passed an instance of self as the first
   // argument.
   static const luaL_Reg methods[] = {
      {"input",        LuaEntity::in},
      {"out",          LuaEntity::out},
      {"getMeta",      LuaEntity::getMeta},
      {"setMeta",      LuaEntity::setMeta},
      {"getMessage",   LuaEntity::getMessage},
      {"setMessage",   LuaEntity::setMessage},
      {"isType",       LuaEntity::isType},
      {"getType",      LuaEntity::getType},
      {"getName",      LuaEntity::getName},
      {"getTitle",     LuaEntity::getTitle},
      {"setTitle",     LuaEntity::setTitle},
      {"getLongDesc",  LuaEntity::getLongDesc},
      {"setLongDesc",  LuaEntity::setLongDesc},
      {"getShortDesc", LuaEntity::getShortDesc},
      {"setShortDesc", LuaEntity::setShortDesc},
      {"observe",      LuaEntity::observe},
      {"glance",       LuaEntity::glance},
      {"observedBy",   LuaEntity::observedBy},
      {"glancedBy",    LuaEntity::glancedBy},
      {"__tostring",   LuaEntity::getName},
      {"__gc",         LuaEntity::gcEntity},
      {0, 0}
   };

   /***************************************************************************/

   const luaL_Reg *LuaEntity::getFunctions() {

      return functions;
   }

   /***************************************************************************/

   const luaL_Reg *LuaEntity::getMethods() {

      return methods;
   }

   /***************************************************************************/

   void LuaEntity::registerLuaType(lua_State *L) {

      luaL_newmetatable(L, MetatableName);

      // Entity.__index = Entity
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      LuaState::luaL_register_wrapper(L, 0, methods);
      LuaState::luaL_register_wrapper(L, PackageName, functions);
   }

   /***************************************************************************/

   Entity *LuaEntity::checkEntity(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Entity **)LuaState::luaL_checkudata_ex(L, i, entityTypes);
   }

   /***************************************************************************/

   int LuaEntity::gcEntity(lua_State *L) {

      Entity *e = checkEntity(L, -1);

      // Entity is not owned by a game, so its allocation is managed solely by
      // Lua and should therefore be subject to garbage collection
      if (!e->getGame()) {

         // Make sure to call the appropriate destructor
         switch (e->getType()) {

            case ENTITY_ROOM:
               delete static_cast<Room *>(e);
               break;

            case ENTITY_OBJECT:
               delete static_cast<Object *>(e);
               break;

            case ENTITY_PLAYER:
               delete static_cast<Player *>(e);
               break;

            case ENTITY_CREATURE:
               delete static_cast<Creature *>(e);
               break;
         }
      }

      return 0;
   }

   /***************************************************************************/

   int LuaEntity::getEntity(lua_State *L) {

      string name = luaL_checkstring(L, -1);

      lua_getglobal(L, LuaGame::globalName);
      Game *g = LuaGame::checkGame(L, -1);

      Entity *e = g->getEntity(name);

      if (e) {
         LuaState::pushEntity(L, e);
      } else {
         lua_pushnil(L);
      }

      return 1;
   }

   /***************************************************************************/

   int LuaEntity::in(lua_State *L) {

      string str;

      int n = lua_gettop(L);

      if (n != 1) {
         return luaL_error(L, "takes no arguments");
      }

      Entity *e = checkEntity(L, -n);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      e->in() >> str;

      lua_pushstring(L, str.c_str());
      return 1;
   }

   /***************************************************************************/

   int LuaEntity::out(lua_State *L) {

      const char *message;
      const char *channel;

      int n = lua_gettop(L);

      if (n > 3) {
         return luaL_error(L, "too many arguments");
      }

      if (n < 2) {
         message = "\n";
      } else {
         message = luaL_checkstring(L, 1 - n);
      }

      if (n < 3) {
         channel = "notifications";
      } else {
         channel = luaL_checkstring(L, -1);
      }

      Entity *e = checkEntity(L, -n);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      e->out(channel) << message;
      e->out(channel).flush();

      return 0;
   }

   /***************************************************************************/

   int LuaEntity::getMeta(lua_State *L) {

      int n = lua_gettop(L);

      if (2 != n) {
         return luaL_error(L, "takes one string argument");
      }

      Entity *e = checkEntity(L, -2);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      lua_pushstring(L, e->getMeta(luaL_checkstring(L, -1)).c_str());
      return 1;
   }

   /***************************************************************************/

   int LuaEntity::setMeta(lua_State *L) {

      int n = lua_gettop(L);

      if (3 != n) {
         return luaL_error(L, "takes two string arguments");
      }

      Entity *e = checkEntity(L, -3);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      e->setMeta(luaL_checkstring(L, -2), luaL_checkstring(L, -1));
      return 0;
   }

   /***************************************************************************/

   int LuaEntity::getMessage(lua_State *L) {

      int n = lua_gettop(L);

      if (2 != n) {
         return luaL_error(L, "takes one string argument");
      }

      Entity *e = checkEntity(L, -2);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      lua_pushstring(L, e->getMessage(luaL_checkstring(L, -1)).c_str());
      return 1;
   }

   /***************************************************************************/

   int LuaEntity::setMessage(lua_State *L) {

      int n = lua_gettop(L);

      if (3 != n) {
         return luaL_error(L, "takes two string arguments");
      }

      Entity *e = checkEntity(L, -3);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      e->setMessage(luaL_checkstring(L, -2), luaL_checkstring(L, -1));
      return 0;
   }

   /***************************************************************************/

   int LuaEntity::isType(lua_State *L) {

      int n = lua_gettop(L);

      if (2 != n) {
         return luaL_error(L, "takes only one input, an entity type");
      }

      Entity *e = checkEntity(L, -2);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      enum EntityType type = Entity::strToType(luaL_checkstring(L, -1));

      lua_pushboolean(L, e->isType(type));
      return 1;
   }

   /***************************************************************************/

   int LuaEntity::getType(lua_State *L) {

      int n = lua_gettop(L);

      if (1 != n) {
         return luaL_error(L, "getType takes no arguments");
      }

      Entity *e = checkEntity(L, 1);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      lua_pushstring(L, e->getTypeName().c_str());
      return 1;
   }

   /***************************************************************************/

   int LuaEntity::getName(lua_State *L) {

      int n = lua_gettop(L);

      if (1 != n) {
         return luaL_error(L, "getName takes no arguments");
      }

      Entity *e = checkEntity(L, 1);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      lua_pushstring(L, e->getName().c_str());
      return 1;
   }

   /***************************************************************************/

   int LuaEntity::getTitle(lua_State *L) {

      int n = lua_gettop(L);

      if (1 != n) {
         return luaL_error(L, "getTitle takes no arguments");
      }

      Entity *e = checkEntity(L, 1);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      lua_pushstring(L, e->getTitle().c_str());
      return 1;
   }

   /***************************************************************************/

   int LuaEntity::setTitle(lua_State *L) {

      int n = lua_gettop(L);

      if (2 != n) {
         return luaL_error(L, "requires one string argument");
      }

      Entity *e = checkEntity(L, -n);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      string title = luaL_checkstring(L, -1);
      e->setTitle(title);

      return 0;
   }

   /***************************************************************************/

   int LuaEntity::getLongDesc(lua_State *L) {

      int n = lua_gettop(L);

      if (1 != n) {
         return luaL_error(L, "getLongDesc takes no arguments");
      }

      Entity *e = checkEntity(L, 1);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      lua_pushstring(L, e->getLongDescription().c_str());
      return 1;
   }

   /***************************************************************************/

   int LuaEntity::setLongDesc(lua_State *L) {

      int n = lua_gettop(L);

      if (2 != n) {
         return luaL_error(L, "requires one string argument");
      }

      Entity *e = checkEntity(L, -2);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      string desc = luaL_checkstring(L, -1);
      e->setLongDescription(desc);

      return 0;
   }

   /***************************************************************************/

   int LuaEntity::getShortDesc(lua_State *L) {

      int n = lua_gettop(L);

      if (1 != n) {
         return luaL_error(L, "getShortDesc takes no arguments");
      }

      Entity *e = checkEntity(L, 1);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      lua_pushstring(L, e->getShortDescription().c_str());
      return 1;
   }

   /***************************************************************************/

   int LuaEntity::setShortDesc(lua_State *L) {

      int n = lua_gettop(L);

      if (2 != n) {
         return luaL_error(L, "requires one string argument");
      }

      Entity *e = checkEntity(L, -2);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      string desc = luaL_checkstring(L, -1);
      e->setShortDescription(desc);

      return 0;
   }

   /***************************************************************************/

   int LuaEntity::observe(lua_State *L) {

      // default values for optional arguments
      bool triggerEvents = true;
      bool displayFull = false;

      int n = lua_gettop(L);

      if (n < 2) {
         return luaL_error(L, "requires at least one string argument");
      }

      else if (n > 4) {
         return luaL_error(L, "too many arguments");
      }

      Entity *observed = LuaEntity::checkEntity(L, -n);
      Being  *observer = LuaBeing::checkBeing(L, 1 - n);

      if (n > 2) {
         triggerEvents = lua_toboolean(L, 2 - n);
      }

      if (n > 3) {
         displayFull = lua_toboolean(L, 3 - n);
      }

      observed->observe(observer, triggerEvents, displayFull);
      return 0;
   }

   /***************************************************************************/

   int LuaEntity::glance(lua_State *L) {

      // default values for optional argument
      bool triggerEvents = true;

      int n = lua_gettop(L);

      if (n < 2) {
         return luaL_error(L, "requires one string argument");
      }

      else if (n > 3) {
         return luaL_error(L, "too many arguments");
      }

      Entity *observed = LuaEntity::checkEntity(L, -n);
      Being  *observer = LuaBeing::checkBeing(L, 1 - n);

      if (n > 2) {
         triggerEvents = lua_toboolean(L, 2 - n);
      }

      observed->glance(observer, triggerEvents);
      return 0;
   }

   /***************************************************************************/

   int LuaEntity::observedBy(lua_State *L) {

      int n = lua_gettop(L);

      if (n < 2) {
         return luaL_error(L, "requires one argument");
      }

      else if (n > 2) {
         return luaL_error(L, "too many arguments");
      }

      Entity *observed = LuaEntity::checkEntity(L, -2);
      Being  *observer = LuaBeing::checkBeing(L, -1);

      lua_pushboolean(L, observed->observedBy(observer));
      return 1;
   }

   /***************************************************************************/

   int LuaEntity::glancedBy(lua_State *L) {

      int n = lua_gettop(L);

      if (n < 2) {
         return luaL_error(L, "requires one argument");
      }

      else if (n > 2) {
         return luaL_error(L, "too many arguments");
      }

      Entity *glanced = LuaEntity::checkEntity(L, -2);
      Being  *glancer = LuaBeing::checkBeing(L, -1);

      lua_pushboolean(L, glanced->glancedBy(glancer));
      return 1;
   }
}}

