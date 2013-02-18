#ifndef LUATABLE_H
#define LUATABLE_H


#include <string>
#include <iterator>
#include <boost/unordered_map.hpp>

using namespace std;
using namespace boost;


namespace core {


   /*
      The LuaTable class represents a Lua table structure.  We use this helper
      class to construct tables, and then pass it to LuaState::pushArgument(),
      which will handle all the work of actually pushing all the fields of the
      table onto the Lua stack.  Nested tables are supported.
   */
   class LuaTable {

      public:

         typedef unordered_map<string, string> StringTable;
         typedef unordered_map<string, double> NumberTable;
         typedef unordered_map<string, bool> BoolTable;
         typedef unordered_map<string, LuaTable> LuaTableTable;

      private:

         // Hash tables mapping field names to values (one for each type)
         StringTable    strings;
         NumberTable    numbers;
         BoolTable      bools;
         LuaTableTable  tables;

      public:

         /*
            setField() sets a value for the specified key.  Each overloaded
            method will populate the appropriate hash table.  Note: there's no
            implemented way to check for setting different value types with the
            same key name (for example, a string and a double, both under the
            key "value".  If such a thing occurs, one will ultimately be
            overwritten by the other when it's pushed onto the Lua stack, and
            which one happens to overwrite the other is undefined.

            Input:
               key (name of the field -- string)
               value (value to be paired with the key -- varied type)
         */
         inline void setField(string key, const char *value) {strings[key] = value;}
         inline void setField(string key, string value) {strings[key] = value;}
         inline void setField(string key, double value) {numbers[key] = value;}
         inline void setField(string key, bool value) {bools[key] = value;}
         inline void setField(string key, LuaTable value) {tables[key] = value;}

         /*
            Returns const references for each hash table.  This is used by
            LuaState when it retrieves and sets these values on the Lua stack.

            Input:
               (none)

            Output:
               Iterators over all set key => value pairs
         */
         inline const StringTable   &getStrings() const {return strings;}
         inline const NumberTable   &getNumbers() const {return numbers;}
         inline const BoolTable     &getBools()   const {return bools;}
         inline const LuaTableTable &getTables()  const {return tables;}
   };
}


#endif

