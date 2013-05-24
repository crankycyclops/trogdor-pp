#include <cstdio>

#include "../include/entities/thing.h"
#include "../include/game.h"


using namespace std;

namespace core { namespace entity {


   LuaTable *Thing::getLuaTable() const {

      LuaTable *table = Entity::getLuaTable();

      if (0 == location) {
         table->setField("location", false);
      }

      else {
         table->setField("location", location->getName());
      }

      stringstream s;
      LuaTable *aliasArr = new LuaTable();

      for (int i = 0; i < aliases.size(); i++) {
         s.str(std::string()), s << i;
         aliasArr->setField(s.str(), aliases[i]);
      }

      table->setField("aliases", *aliasArr);

      return table;
   }

   /***************************************************************************/

   void Thing::display(Being *observer, bool displayFull) {

      *game->trogout << "You see " << getTitle() << '.' << endl;
      Entity::display(observer, displayFull);
   }

   /***************************************************************************/

   void Thing::displayShort(Being *observer) {

      *game->trogout << "You see " << getTitle() << '.' << endl;
      Entity::displayShort(observer);
   }
}}

