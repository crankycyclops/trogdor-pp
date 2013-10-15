#include <cstdio>

#include "../include/entities/thing.h"
#include "../include/entities/item.h"
#include "../include/entities/being.h"
#include "../include/game.h"


using namespace std;

namespace core { namespace entity {


   void Thing::display(Being *observer, bool displayFull) {

      observer->out("display") << "You see " << getTitle() << '.' << endl;
      Entity::display(observer, displayFull);
   }

   /***************************************************************************/

   void Thing::displayShort(Being *observer) {

      observer->out("display") << "You see " << getTitle() << '.' << endl;
      Entity::displayShort(observer);
   }

   /***************************************************************************/

   void Thing::addAlias(string alias) {

      aliases.insert(aliases.end(), alias);

      // NOTE: Item's can be owned, but right now, only objects can be in inventory
      if (isType(ENTITY_OBJECT) && static_cast<Item *>(this)->getOwner()) {
         static_cast<Object *>(this)->getOwner()->indexInventoryItemName(alias, static_cast<Object *>(this));
      }
   }
}}

