#include "../include/entities/thing.h"
#include "../include/game.h"


using namespace std;

namespace core { namespace entity {

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

