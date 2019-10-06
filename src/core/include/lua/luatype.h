#ifndef LUATYPE_H
#define LUATYPE_H


#include <string>
#include <vector>
#include <boost/variant.hpp>

using namespace std;
using namespace boost;


namespace trogdor {


   struct LuaValue;
   class LuaTable;


   enum LuaDataType {
      LUA_TYPE_STRING,
      LUA_TYPE_NUMBER,
      LUA_TYPE_BOOLEAN,
      LUA_TYPE_TABLE,
      LUA_TYPE_ARRAY,
      LUA_TYPE_FUNCTION
   };

   // I couldn't forward declare the vector, so pay attention to this if LuaArray changes
   typedef boost::variant<string, double, bool, vector<LuaValue> *, LuaTable *> LuaValueContent;

   // Represents a single value
   struct LuaValue {
       LuaDataType      type;
       LuaValueContent  value;
   };

   typedef vector<LuaValue> LuaArray;
}


#endif

