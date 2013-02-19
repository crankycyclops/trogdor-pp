#ifndef LUATABLE_H
#define LUATABLE_H


#include <string>
#include <iterator>
#include <boost/unordered_map.hpp>
#include <boost/variant.hpp>

using namespace std;
using namespace boost;


namespace core {


   class LuaTable;   // forward declaration for LuaTableValue


   /*
      Represents a single value, which gets mapped to a key.  Value can be of
      more than one type.
   */
   typedef struct {

      enum {
         LUATABLE_VALUE_STRING,
         LUATABLE_VALUE_NUMBER,
         LUATABLE_VALUE_BOOLEAN,
         LUATABLE_VALUE_TABLE,
         LUATABLE_VALUE_FUNCTION
      } type;

      boost::variant<string, double, bool, LuaTable *> value;

   } LuaTableValue;


   /*
      The LuaTable class represents a Lua table structure.  We use this helper
      class to construct tables, and then pass it to LuaState::pushArgument(),
      which will handle all the work of actually pushing all the fields of the
      table onto the Lua stack.  Nested tables are supported.
   */
   class LuaTable {

      public:

         typedef unordered_map<string, LuaTableValue> TableValues;

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

            LuaTableValue v;
            v.type = LuaTableValue::LUATABLE_VALUE_STRING;
            string s = value;
            v.value = s;

            values[key] = v;
         }

         inline void setField(string key, string value) {

            LuaTableValue v;
            v.type = LuaTableValue::LUATABLE_VALUE_STRING;
            v.value = value;

            values[key] = v;
         }

         inline void setField(string key, int value) {

            LuaTableValue v;
            v.type = LuaTableValue::LUATABLE_VALUE_NUMBER;
            v.value = (double)value;

            values[key] = v;
         }

         inline void setField(string key, double value) {

            LuaTableValue v;
            v.type = LuaTableValue::LUATABLE_VALUE_NUMBER;
            v.value = value;

            values[key] = v;
         }

         inline void setField(string key, bool value) {

            LuaTableValue v;
            v.type = LuaTableValue::LUATABLE_VALUE_BOOLEAN;
            v.value = value;

            values[key] = v;
         }

         inline void setField(string key, LuaTable &value) {

            LuaTableValue v;
            v.type = LuaTableValue::LUATABLE_VALUE_TABLE;
            v.value = &value;

            values[key] = v;
         }

         inline void setFieldFunction(string key, const char *func) {

            LuaTableValue v;
            v.type = LuaTableValue::LUATABLE_VALUE_FUNCTION;
            v.value = func;

            values[key] = v;
         }

         inline void setFieldFunction(string key, string func) {

            LuaTableValue v;
            v.type = LuaTableValue::LUATABLE_VALUE_FUNCTION;
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
         inline const TableValues   &getValues()  const {return values;}
   };
}


#endif

