#include "../include/game.h"
#include "../include/entities/object.h"


using namespace std;
using namespace boost;

namespace core { namespace entity {


   LuaTable *Object::getLuaTable() const {

      LuaTable *table = Item::getLuaTable();

      table->setField("weight", weight);

      return table;
   }
}}

