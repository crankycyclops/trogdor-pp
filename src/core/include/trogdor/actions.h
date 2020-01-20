#ifndef ACTIONS_H
#define ACTIONS_H


#include <memory>
#include <trogdor/action.h>

namespace trogdor {


   /*
      The Quit action removes the player who issued the command from the game.
   */
   class QuitAction: public Action {

      public:

         /*
            See documentation in action.h.  A valid syntax for the Quit action
            is to have a command with just a verb and no direct object or
            indirect object.
         */
         virtual bool checkSyntax(const std::shared_ptr<Command> &command);

         virtual void execute(Player *player, const std::shared_ptr<Command> &command, Game *game);
   };

/******************************************************************************/

   /*
      The Cuss action responds to players when they cuss ;)
   */
   class CussAction: public Action {

      public:

         virtual bool checkSyntax(const std::shared_ptr<Command> &command);

         virtual void execute(Player *player, const std::shared_ptr<Command> &command, Game *game);
   };

/******************************************************************************/

   /*
      The Inventory action lists the items in a Player's inventory.
   */
   class InventoryAction: public Action {

      public:

         /*
            See documentation in action.h.  A valid syntax for the Inventory
            action is to have a command with just a verb and no direct object or
            indirect object.
         */
         virtual bool checkSyntax(const std::shared_ptr<Command> &command);

         virtual void execute(Player *player, const std::shared_ptr<Command> &command, Game *game);
   };

/******************************************************************************/

   class LookAction: public Action {

         /*
            See documentation in action.h.  A valid syntax for the Look action
            is to have a command with just a verb or one of a direct or indirect
            object.  Just the look verb with no indirect object will observe the
            player's current location.  Looking at a direct object will result
            in the player observing that object (if it exists in the user's
            current Location OR inventory.)
         */
         virtual bool checkSyntax(const std::shared_ptr<Command> &command);

         virtual void execute(Player *player, const std::shared_ptr<Command> &command, Game *game); 
   };

/******************************************************************************/

   class TakeAction: public Action {

         /*
            See documentation in action.h.  A valid syntax for the Take action
            is to have a verb + direct object.  The result is for the Object
            to be taken into the Being's inventory, if possible.
         */
         virtual bool checkSyntax(const std::shared_ptr<Command> &command);

         virtual void execute(Player *player, const std::shared_ptr<Command> &command, Game *game); 
   };

/******************************************************************************/

   class DropAction: public Action {

         /*
            See documentation in action.h.  A valid syntax for the Drop action
            is to have a verb + direct object.  The result is for the Object
            to be removed from the Being's inventory and dropped into the Being's
            current location, if possible.
         */
         virtual bool checkSyntax(const std::shared_ptr<Command> &command);

         virtual void execute(Player *player, const std::shared_ptr<Command> &command, Game *game); 
   };

/******************************************************************************/

   /*
      The Move action allows a player to move from one room to another.
   */
   class MoveAction: public Action {

      public:

         /*
            See documentation in action.h.  A valid syntax for the Move action
            is to have a verb and either a direct object or indirect object
            (but not both) that matches a valid direction name, or a verb that
            matches a valid direction name.
         */
         virtual bool checkSyntax(const std::shared_ptr<Command> &command);

         virtual void execute(Player *player, const std::shared_ptr<Command> &command, Game *game);
   };

/******************************************************************************/

   /*
      The Attack action allows a player to attack another Being.
   */
   class AttackAction: public Action {

      public:

         /*
            See documentation in action.h.  A direct object is required.  An
            indirect object is optional, and would specify a weapon to use in
            the attack.
         */
         virtual bool checkSyntax(const std::shared_ptr<Command> &command);

         virtual void execute(Player *player, const std::shared_ptr<Command> &command, Game *game);
   };
}


#endif
