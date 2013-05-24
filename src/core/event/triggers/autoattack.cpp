#include "../../include/event/triggers/autoattack.h"

using namespace std;

namespace core { namespace event {


   void AutoAttackEventTrigger::execute(EventArgumentList args) {

      Being *being = static_cast<Being *>(boost::get<Entity *>(args[0]));
      Place *place = static_cast<Place *>(boost::get<Entity *>(args[2]));

      // each Creature that has auto-attack enabled should be setup to attack
      for (entity::CreatureListCIt i = place->getCreaturesBegin();
      i != place->getCreaturesEnd(); i++) {

         if ((*i)->getAutoAttackEnabled()) {
            // TODO
            cout << "STUB: " << (*i)->getName() << " auto attack commence!" << endl;
         }
      }

      continueExecutionFlag = true;
      allowActionFlag = true;
   }
}}

