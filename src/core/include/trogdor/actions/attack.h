#ifndef ATTACK_ACTION_H
#define ATTACK_ACTION_H


#include <random>

#include <trogdor/entities/player.h>
#include <trogdor/actions/action.h>


namespace trogdor {


   // The Attack action allows a player to attack another Being.
   class AttackAction: public Action {

      private:

         // If a player is executing this action from an input interceptor that
         // attempts to pick one Thing to look at from a list of available
         // options with the same alias, make sure we lookup the item by its
         // name rather than its alias to avoid infinite calls to clarifyEntity()
         // in the case where the desired object's name matches an alias with
         // more than one Thing.
         std::unordered_map<
            entity::Player *,
            std::weak_ptr<entity::Player>
         > lookupDefenderByName;

         std::unordered_map<
            entity::Player *,
            std::weak_ptr<entity::Player>
         > lookupWeaponByName;

         // The player has decided to kill himself. What a shame.
         inline void commitSuicide(entity::Player *player) {

            static const char *suicideResponses[] = {
               "Done.",
               "If you insist...",
               "You know, they have hotlines for that sort of thing.",
               "Was life really so hard?",
               "I hope you left a note.",
               "You will be missed.",
               "Rest in peace."
            };

            static int arrSize = sizeof(suicideResponses) / sizeof (const char *);

            static std::random_device rd;
            static std::minstd_rand generator(rd());
            static std::uniform_int_distribution<unsigned> distribution(0, arrSize - 1);

            if (player->isAlive()) {

               player->out("display") << suicideResponses[distribution(generator)] << std::endl;
               player->die(true);
            }

            else {
               player->out("display") << "You're already dead." << std::endl;
            }
         }

      public:

         /*
            See documentation in action.h.  A direct object is required.  An
            indirect object is optional, and would specify a weapon to use in
            the attack.
         */
         virtual bool checkSyntax(const Command &command);

         virtual void execute(
            entity::Player *player,
            const Command &command,
            Game *game
         );
   };
}


#endif
