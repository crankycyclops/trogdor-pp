#include "../include/game.h"
#include "../include/entities/player.h"


using namespace std;
using namespace boost;

namespace core { namespace entity {


   LuaTable *Player::getLuaTable() const {

      LuaTable *table = Being::getLuaTable();

      // There's nothing here right now, but this is a placeholder in case
      // Players ever do get properties that will need to be added later.

      return table;
   }
}}

