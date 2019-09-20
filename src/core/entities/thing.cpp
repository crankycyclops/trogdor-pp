#include <cstdio>

#include "../include/entities/thing.h"
#include "../include/entities/being.h"
#include "../include/game.h"


using namespace std;

namespace trogdor { namespace entity {


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
   }
}}

