#include "../include/entities/thing.h"
#include "../include/game.h"


using namespace std;

namespace core { namespace entity {

   void Thing::display(Being *observer) {

      *game->trogout << "You see " << getTitle() << endl;
      Entity::display(observer);
   }
}}

