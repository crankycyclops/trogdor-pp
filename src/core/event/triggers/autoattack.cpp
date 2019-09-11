#include <algorithm>

#include "../../include/event/triggers/autoattack.h"
#include "../../include/timer/jobs/autoattack.h"

using namespace std;

namespace trogdor { namespace core { namespace event {


   void AutoAttackEventTrigger::execute(EventArgumentList args) {

      Game  *game  = boost::get<Game *>(args[0]);
      Being *being = static_cast<Being *>(boost::get<Entity *>(args[1]));
      Place *place = static_cast<Place *>(boost::get<Entity *>(args[3]));

      entity::CreatureListCItPair creatures = place->getCreatures();

      // each Creature that has auto-attack enabled should be setup to attack
      for_each(creatures.begin, creatures.end, [&](entity::Creature * const &creature) {

         if (creature->getAutoAttackEnabled()) {

            int in = creature->getAutoAttackInterval();
            int e  = creature->getAutoAttackRepeat() ? -1 : 1;
            int s  = creature->getAutoAttackInterval();

            AutoAttackTimerJob *j = new AutoAttackTimerJob(game, in, e, s, creature, being);
            game->insertTimerJob(j);
         }
      });

      continueExecutionFlag = true;
      allowActionFlag = true;
   }
}}}

