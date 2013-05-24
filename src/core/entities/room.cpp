#include "../include/game.h"
#include "../include/entities/room.h"


using namespace std;
using namespace boost;

namespace core { namespace entity {


   LuaTable *Room::getLuaTable() const {

      LuaTable *table = Place::getLuaTable();

      LuaTable *connectionsArr = new LuaTable();
      for (unordered_map<string, Room *>::const_iterator i = connections.begin();
      i != connections.end(); i++) {
         connectionsArr->setField(i->first, i->second->getName());
      }

      table->setField("connections", *connectionsArr);

      return table;
   }
}}

