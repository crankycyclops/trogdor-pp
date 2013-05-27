#include "../../include/event/triggers/autoattack.h"
#include "../../include/timer/jobs/autoattack.h"

using namespace std;

namespace core { namespace event {


   void AutoAttackEventTrigger::execute(EventArgumentList args) {

      Game  *game  = boost::get<Game *>(args[0]);
      Being *being = static_cast<Being *>(boost::get<Entity *>(args[1]));
      Place *place = static_cast<Place *>(boost::get<Entity *>(args[3]));

      entity::CreatureListCItPair creatures = place->getCreatures();

      // each Creature that has auto-attack enabled should be setup to attack
      for (entity::CreatureListCIt i = creatures.begin; i != creatures.end; i++) {

         if ((*i)->getAutoAttackEnabled()) {

            int in = (*i)->getAutoAttackInterval();
            int e  = (*i)->getAutoAttackRepeat() ? -1 : 1;
            int s  = (*i)->getAutoAttackInterval();

            AutoAttackTimerJob *j = new AutoAttackTimerJob(game, in, e, s, *i, being);
            game->insertTimerJob(j);
         }
      }

      continueExecutionFlag = true;
      allowActionFlag = true;
   }
}}

