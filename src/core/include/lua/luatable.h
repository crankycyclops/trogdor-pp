#ifndef LUATABLE_H
#define LUATABLE_H


#include <iterator>
#include <boost/unordered_map.hpp>

#include "luatype.h"

using namespace std;
using namespace boost;


namespace core {


   class LuaTable;


   /*
      The LuaTable class represents a Lua table structure.  We use this helper
      class to construct tables, and then pass it to LuaState::pushArgument(),
      which will handle all the work of actually pushing all the fields of the
      table onto the Lua stack.  Nested tables are supported.
   */
   class LuaTable {

      public:

         typedef unordered_map<string, LuaValue> TableValues;

      private:

         TableValues values;

      public:

         /*
            setField() sets a value for the specified key.

            Input:
               key (name of the field -- string)
               value (value to be paired with the key -- varied type)
         */
         inline void setField(string key, const char *value) {

            LuaValue v;
            v.type = LUA_TYPE_STRING;
            string s = value;
            v.value = s;

            values[key] = v;
         }

         inline void setField(string key, string value) {

            LuaValue v;
            v.type = LUA_TYPE_STRING;
            v.value = value;

            values[key] = v;
         }

         inline void setField(string key, int value) {

            LuaValue v;
            v.type = LUA_TYPE_NUMBER;
            v.value = (double)value;

            values[key] = v;
         }

         inline void setField(string key, double value) {

            LuaValue v;
            v.type = LUA_TYPE_NUMBER;
            v.value = value;

            values[key] = v;
         }

         inline void setField(string key, bool value) {

            LuaValue v;
            v.type = LUA_TYPE_BOOLEAN;
            v.value = value;

            values[key] = v;
         }

         inline void setField(string key, LuaTable &value) {

            LuaValue v;
            v.type = LUA_TYPE_TABLE;
            v.value = &value;

            values[key] = v;
         }

         inline void setFieldFunction(string key, const char *func) {

            LuaValue v;
            v.type = LUA_TYPE_FUNCTION;
            v.value = func;

            values[key] = v;
         }

         inline void setFieldFunction(string key, string func) {

            LuaValue v;
            v.type = LUA_TYPE_FUNCTION;
            v.value = func;

            values[key] = v;
         }

         /*
            Returns const reference to our hash table of values.  This is used
            by LuaState when it retrieves and sets these values on the Lua stack.

            Input:
               (none)

            Output:
               Const references to hash table
         */
         inline const TableValues &getValues() const {return values;}
   };
}


#endif

