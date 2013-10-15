#include "../include/game.h"
#include "../include/entities/object.h"


using namespace std;
using namespace boost;

namespace core { namespace entity {


   void Object::addAlias(string alias) {

      Thing::addAlias(alias);

      if (owner) {
         owner->indexInventoryItemName(alias, this);
      }
   }
}}

