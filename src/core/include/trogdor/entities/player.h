#ifndef PLAYER_H
#define PLAYER_H


#include <memory>
#include <functional>

#include <trogdor/entities/being.h>


namespace trogdor::entity {


   class Player: public Being {

      private:

         // Input always flows one way, from the application to the library.
         // If a command requires further input from the user, there's no input
         // stream the library can read from. Instead, we can set a temporary
         // callback that Player::input() will execute on the next call instead
         // of Game::executeAction().
         std::unique_ptr<std::function<bool(std::string)>> inputInterceptor = nullptr;

         // The player's last executed command
         std::unique_ptr<Command> lastCommand;

      public:

         /*
            Constructor for creating a new Player.  Requires reference to the
            containing Game object and a name.
         */
         Player(Game *g, std::string n, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogerr> e);

         /*
            Constructor for cloning an existing player.  Requires a unique name.
         */
         Player(const Player &p, std::string n, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogerr> e);

         /*
            This constructor deserializes a Player. Unlike the other Entity
            deserialization constructors, this one still requires output and
            error stream arguments.

            Input:
               Raw deserialized data (const Serializable &)
               Output stream (std::unique_ptr<Trogout> o)
               Error stream (std::unique_ptr<Trogerr> e)
         */
         Player(const serial::Serializable &data, std::unique_ptr<Trogout> o,
         std::unique_ptr<Trogerr> e);

         /*
            Returns a smart pointer representing a raw Player pointer. Be careful
            with this and only call it on Entities you know are managed by smart
            pointers. If, for example, you call this method on entities that are
            managed by Lua using new and delete, you're going to have a bad time.

            Input:
               (none)

            Output:
               std::shared_ptr<Player>
         */
         inline std::shared_ptr<Player> getShared() {

            return std::dynamic_pointer_cast<Player>(Entity::getShared());
         }

         /*
            If some other part of the library requires further input from the
            user, this callback will allow that part of the library to hijack
            our usual call to Game::executeAction() via this callback. The
            callback is executed once, then unset.

            Input:
               Callback

            Output:
               (none)
         */
         inline void setInputInterceptor(
            std::unique_ptr<std::function<bool(std::string)>> interceptor
         ) {

            inputInterceptor = std::move(interceptor);
         }

         /*
            Returns the player's last entered command.

            Input:
               (none)

            Output:
               The player's last entered command, or a nullptr if one hasn't
               been entered yet.
         */
         inline const Command *getLastCommand() {return lastCommand.get();}

         /*
            Serializes the Player.

            Input:
               (none)

            Output:
               An object containing easily serializable data (Serializable)
         */
         virtual serial::Serializable serialize();

         /*
            Takes as input a command and executes it on the player's behalf.

            Input:
               Command to execute (std::string)

            Output:
               True if the command was executable (the verb action was found and
               the syntax was valid) and false if not
         */
         bool input(std::string commandStr);
   };
}


#endif
