#ifndef ACTION_H
#define ACTION_H


#include <cmath>

#include <trogdor/game.h>
#include <trogdor/command.h>

#include <trogdor/entities/tangible.h>
#include <trogdor/entities/resource.h>
#include <trogdor/entities/player.h>


namespace trogdor {

   namespace entity {

      // Forward declaration to prevent circular dependencies
      class Player;
   }


   /*
      This abstract class models a specific action in the game.  The verb in
      a user's command determines which action is executed.  All actions
      inherit from this class, which provides virtual functions that must be
      uniquely implemented for each action.
   */
   class Action {

      public:

         /*
            When a class has one or more virtual functions, it should also have
            a virtual destructor.
         */
         virtual ~Action() = 0;

         /*
            Checks the syntax of a command.  Returns true if the syntax is valid
            and false if it is not.  It's up to the specific action class to
            implement this method.

            Input:
               Command
            Output:
               bool
         */
         virtual bool checkSyntax(const Command &command) = 0;

         /*
            Executes the action.  This mehod will be implemented by the specific
            action.

            Input:
               Player executing command
               Command
               Game in which command is made (Game *)
            Output:
               (none)
         */
         virtual void execute(
            entity::Player *player,
            const Command &command,
            Game *game
         ) = 0;

         /*
            Since a lot of actions will require similar code, this is provided
            here as a utility method. Takes as input a resource, the name the
            player typed as part of a command, the maximum amount of the
            resource available, and the amount of the resource the user typed
            as part of a command (might not have a value) and returns a
            disambiguated amount of the resource that should be acted upon.

            To illustrate this method's use, consider the following commands:

            Singular:

            "take gold coin"
            "take 1 gold coin"

            In both examples, we should only be taking one coin, but only in the
            second command is the value of 1 explicit.

            Plural:

            "take gold coins"

            In this example, we know the command is plural, but we don't know
            how many the user wants to take. We disambiguate by assuming the
            player wants to take all of the resource that's available.

            Input:
               Resource (entity::Resource *)
               Singular or plural resource name as entered by the user (std::string)
               Maximum amount of the resource available (double)
               Explicit amount of the resource entered by the user, which might
                  not have a value (std::optional<double>)

            Output:
               The amount of resource implied by the player's command (double)
         */
         inline double resolveResourceAmount(
            entity::Resource *resource,
            std::string name,
            double max,
            std::optional<double> qty = std::nullopt
         ) const {

            if (qty) {
               return *qty;
            }

            else if (
               resource->isPlural(name) ||
               !resource->areIntegerAllocationsRequired()
            ) {
               return max;
            }

            else {
               return 1;
            }
         }

         /*
            If an action needs to operate on a resource due to player input,
            this utility method should be called. This method takes as input
            the resource, the entity that holds the resource, the player, the
            amount of the resource we're using, and a callback that should be
            executed if the operation can be performed.

            Input:
               Resource (entity::Resource *)
               The entity holding the resource (entity::Tangible *)
               The player who entered the command (entity::Player *)
               The amount of the resource we're operating on (double)
               The callback to execute if the operation is allowed to proceed (std::function)

            Output:
               (none)
         */
         inline void operateOnResource(
            entity::Resource *resource,
            entity::Tangible *depositor,
            entity::Player *player,
            double amount,
            std::function<void()> operation
         ) {

            auto i = depositor->getResources().find(resource->getShared());

            if (i != depositor->getResources().end()) {

               auto allocation = *i;
               double intPart, fracPart = modf(amount, &intPart);

               if (amount <= 0) {
                  player->out("display") << "Please specify an amount greater than zero."
                     << std::endl;
               }

               else if (resource->areIntegerAllocationsRequired() && fracPart) {
                  player->out("display") << "Please specify a whole number of "
                     << resource->getPluralTitle() << '.' << std::endl;
               }

               else if (amount > allocation.second) {

                  std::string title = resource->getPluralTitle();

                  if (
                     resource->areIntegerAllocationsRequired() &&
                     1 == std::lround(allocation.second)
                  ) {
                     title = resource->getTitle();
                  }

                  std::string qtyStr = std::to_string(allocation.second);

                  if (resource->areIntegerAllocationsRequired()) {
                     qtyStr = std::to_string(std::lround(allocation.second));
                  }

                  if (static_cast<entity::Tangible *>(player) == depositor) {
                     player->out("display") << "You only have " << qtyStr
                        << ' ' << title << '.' << std::endl;
                  }

                  else {
                     player->out("display") << "There are only " << qtyStr
                        << ' ' << title << '.' << std::endl;
                  }
               }

               else {
                  operation();
               }
            }
         }
   };
}


#endif
