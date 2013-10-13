#include "../../include/entities/entity.h"
#include "../../include/lua/entities/luaentity.h"

using namespace std;

namespace core { namespace entity {


   const luaL_reg *LuaEntity::getFunctions() {

      // functions that take an Entity as an input (new, get, etc.)
      // format of call: Entity.new(e), where e is an Entity
      static const luaL_reg functions[] = {
         {0, 0}
      };

      return functions;
   }

   const luaL_reg *LuaEntity::getMethods() {

      // Lua Entity methods that bind to C++ Entity methods
      // also includes meta methods
      static const luaL_reg methods[] = {
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
         {"__tostring",   LuaEntity::getName},
         {0, 0}
      };

      return methods;
   }

   int LuaEntity::getMethodCount() {

      const luaL_reg *methods = getMethods();
      int count = 0;

      for (luaL_reg func = methods[0]; func.name != 0; func = methods[count]) {
         count++;
      }

      return count;
   }

   void LuaEntity::registerLuaType(lua_State *L) {

      const luaL_reg *functions = getFunctions();
      const luaL_reg *methods = getMethods();

      luaL_newmetatable(L, "Entity");

      // Entity.__index = Entity
      lua_pushvalue(L, -1);
      lua_setfield(L, -2, "__index");

      luaL_register(L, 0, methods);
      luaL_register(L, "Entity", functions);
   }


   Entity *LuaEntity::checkEntity(lua_State *L, int i) {

      luaL_checktype(L, i, LUA_TUSERDATA);
      return *(Entity **)luaL_checkudata(L, i, "Entity");
   }


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

      return 1;
   }


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
      return 1;
   }


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
      return 1;
   }


   int LuaEntity::isType(lua_State *L) {

      int n = lua_gettop(L);

      if (2 != n) {
         return luaL_error(L, "takes only one input, an entity type");
      }

      Entity *e = checkEntity(L, -2);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      enum EntityType type;
      string typeStr = luaL_checkstring(L, -1);

      // TODO: move all of this into Entity::strToType()
      if (0 == typeStr.compare("entity")) {
         type = ENTITY_UNDEFINED;
      }

      else if (0 == typeStr.compare("place")) {
         type = ENTITY_PLACE;
      }

      else if (0 == typeStr.compare("room")) {
         type = ENTITY_ROOM;
      }

      else if (0 == typeStr.compare("thing")) {
         type = ENTITY_THING;
      }

      else if (0 == typeStr.compare("item")) {
         type = ENTITY_ITEM;
      }

      else if (0 == typeStr.compare("object")) {
         type = ENTITY_OBJECT;
      }

      else if (0 == typeStr.compare("being")) {
         type = ENTITY_BEING;
      }

      else if (0 == typeStr.compare("player")) {
         type = ENTITY_PLAYER;
      }

      else if (0 == typeStr.compare("creature")) {
         type = ENTITY_CREATURE;
      }

      // string doesn't match any type (script writer is stupid)
      else {
         lua_pushboolean(L, 0);
         return 1;
      }

      lua_pushboolean(L, e->isType(type));
      return 1;
   }


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


   int LuaEntity::setTitle(lua_State *L) {

      int n = lua_gettop(L);

      if (3 != n) {
         return luaL_error(L, "takes one string argument");
      }

      Entity *e = checkEntity(L, -2);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      string title = luaL_checkstring(L, -1);
      e->setTitle(title);

      return 1;
   }


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


   int LuaEntity::setLongDesc(lua_State *L) {

      int n = lua_gettop(L);

      if (3 != n) {
         return luaL_error(L, "takes one string argument");
      }

      Entity *e = checkEntity(L, -2);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      string desc = luaL_checkstring(L, -1);
      e->setLongDescription(desc);

      return 1;
   }


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


   int LuaEntity::setShortDesc(lua_State *L) {

      int n = lua_gettop(L);

      if (3 != n) {
         return luaL_error(L, "takes one string argument");
      }

      Entity *e = checkEntity(L, -2);

      if (0 == e) {
         return luaL_error(L, "not an Entity!");
      }

      string desc = luaL_checkstring(L, -1);
      e->setShortDescription(desc);

      return 1;
   }
}}

