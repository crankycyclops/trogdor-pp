#ifndef LUATYPE_H
#define LUATYPE_H


#include <string>
#include <boost/variant.hpp>

using namespace std;
using namespace boost;


namespace core {


   class LuaArray;
   class LuaTable;


   typedef enum {
      LUA_TYPE_STRING,
      LUA_TYPE_NUMBER,
      LUA_TYPE_BOOLEAN,
      LUA_TYPE_TABLE,
      LUA_TYPE_ARRAY,
      LUA_TYPE_FUNCTION
   } LuaDataType;

   typedef boost::variant<string, double, bool, LuaArray *, LuaTable *> LuaValueContent;

   // Represents a single value
   typedef struct {
       LuaDataType      type;
       LuaValueContent  value;
   } LuaValue;
}


#endif

