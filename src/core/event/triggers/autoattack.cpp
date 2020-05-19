#include <memory>

#include <trogdor/event/triggers/autoattack.h>
#include <trogdor/timer/jobs/autoattack.h>

namespace trogdor::event {


   void AutoAttackEventTrigger::execute(EventArgumentList args) {

      Game  *game  = std::get<Game *>(args[0]);
      Being *being = static_cast<Being *>(std::get<Entity *>(args[1]));
      Place *place = static_cast<Place *>(std::get<Entity *>(args[3]));

      // each Creature that has auto-attack enabled should be setup to attack
      for (auto const &creature: place->getCreatures()) {

         if (creature->getAutoAttackEnabled()) {

            std::shared_ptr<AutoAttackTimerJob> j = std::make_shared<AutoAttackTimerJob>(
               game,
               creature->getAutoAttackInterval(),
               creature->getAutoAttackRepeat() ? -1 : 1,
               creature->getAutoAttackInterval(),
               creature,
               being
            );

            game->insertTimerJob(j);
         }
      };

      continueExecutionFlag = true;
      allowActionFlag = true;
   }
}
