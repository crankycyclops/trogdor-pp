#include <random>

#include <trogdor/entities/player.h>
#include <trogdor/actions/cuss.h>

namespace trogdor {


   bool CussAction::checkSyntax(const Command &command) {

      return true;
   }

   /***************************************************************************/

   void CussAction::execute(
      entity::Player *player,
      const Command &command,
      Game *game
   ) {

      static const char *responses[] = {
         "Such language!",
         "You, sir, have a foul mouth!",
         "Well, ?&*@! to you, too!",
         "Do you kiss your mother with that mouth?",
         "Classy.",
         "Swear much?"
      };

      static int arrSize = sizeof(responses) / sizeof (const char *);

      static std::random_device rd;
      static std::minstd_rand generator(rd());
      static std::uniform_int_distribution<unsigned> distribution(0, arrSize - 1);

      player->out("display") << responses[distribution(generator)] << std::endl;
   }
}
