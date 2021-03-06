#include <trogdor/game.h>
#include <trogdor/entities/player.h>


namespace trogdor::entity {

   Player::Player(Game *g, std::string n, std::unique_ptr<Trogout> o,
   std::unique_ptr<Trogerr> e): Being(g, n, std::move(o), std::move(e)),
   lastCommand(std::make_unique<Command>(game->getVocabulary(), "")) {

      types.push_back(ENTITY_PLAYER);
      setClass("player");

      setProperty("longDesc", name + " is a player.");
   }

   /**************************************************************************/

   Player::Player(
      const Player &p,
      std::string n,
      std::unique_ptr<Trogout> o,
      std::unique_ptr<Trogerr> e
   ): Being(p, n), lastCommand(std::make_unique<Command>(*(p.lastCommand))) {

      // TODO: Do I need this? I already do this in Entity's constructor
      setProperty("title", n);

      outStream = std::move(o);
      errStream = std::move(e);

      setProperty("longDesc", name + " is a player.");
   }

   /***************************************************************************/

   Player::Player(
      Game *g,
      const serial::Serializable &data,
      std::unique_ptr<Trogout> o,
      std::unique_ptr<Trogerr> e
   ): Being(g, data, std::move(o), std::move(e)) {

      if (auto serializedLastCommand = data.get("lastCommand")) {
         lastCommand = std::make_unique<Command>(
            game->getVocabulary(),
            *std::get<std::shared_ptr<serial::Serializable>>(*serializedLastCommand)
         );
      }

      // We reach this block of code when we're deserializing the default player
      // template instance
      else {
         lastCommand = std::make_unique<Command>(game->getVocabulary(), "");
      }

      types.push_back(ENTITY_PLAYER);
   }

   /***************************************************************************/

   std::shared_ptr<serial::Serializable> Player::serialize() {

      std::shared_ptr<serial::Serializable> data = Being::serialize();

      data->set("lastCommand", lastCommand->serialize());
      return data;
   }

   /**************************************************************************/

   bool Player::input(std::string commandStr) {

      const Vocabulary &vocabulary = game->getVocabulary();
      std::unique_ptr<Command> command = std::make_unique<Command>(vocabulary, commandStr);

      // don't do anything if the user didn't actually enter a command
      if (command->isNull()) {
         return true;
      }

      // do nothing if the game isn't started
      else if (!game->inProgress()) {
         out() << "Game is stopped and not accepting commands." << std::endl;
         return false;
      }

      // Some other part of the library is waiting on input from the Player, so
      // process that instead of an ordinary game command
      else if (inputInterceptor) {

         auto formerInputInterceptor = inputInterceptor.get();
         bool retVal = (*inputInterceptor)(commandStr);

         // Input interceptors are only supposed to fire once, but it's possible
         // that somewhere in the callback, another one was set. If that's the
         // case, the second interceptor should remain set.
         if (formerInputInterceptor == inputInterceptor.get()) {
            inputInterceptor = nullptr;
         }

         return retVal;
      }

      if (!command->isInvalid()) {

         // player is attempting to re-execute his last command
         if (0 == command->getVerb().compare("a") ||
         0 == command->getVerb().compare("again")) {

            if (nullptr != lastCommand) {
               command = std::move(lastCommand);
            }

            else {
               out() << "You haven't entered any commands yet!" << std::endl;
               return false;
            }
         }

         if (!game->executeAction(this, *command)) {
            out() << "Sorry, I don't understand you." << std::endl;
            return false;
         }
      }

      else {
         out() << "Sorry, I don't understand you." << std::endl;
         return false;
      }

      lastCommand = std::move(command);
      return true;
   }
}
