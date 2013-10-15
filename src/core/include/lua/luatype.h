#ifndef LUATYPE_H
#define LUATYPE_H


#include <string>
#include <vector>
#include <boost/variant.hpp>

using namespace std;
using namespace boost;


namespace core {


   struct _LuaValue;
   class LuaTable;


   typedef enum {
      LUA_TYPE_STRING,
      LUA_TYPE_NUMBER,
      LUA_TYPE_BOOLEAN,
      LUA_TYPE_TABLE,
      LUA_TYPE_ARRAY,
      LUA_TYPE_FUNCTION
   } LuaDataType;

   // I couldn't forward declare the vector, so pay attention to this if LuaArray changes
   typedef boost::variant<string, double, bool, vector<struct _LuaValue> *, LuaTable *> LuaValueContent;

   // Represents a single value
   typedef struct _LuaValue {
       LuaDataType      type;
       LuaValueContent  value;
   } LuaValue;

   typedef vector<LuaValue> LuaArray;
}


#endif

