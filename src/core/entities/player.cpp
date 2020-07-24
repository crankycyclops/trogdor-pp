#include <trogdor/game.h>
#include <trogdor/entities/player.h>


namespace trogdor::entity {

   Player::Player(Game *g, std::string n, std::unique_ptr<Trogout> o,
   std::unique_ptr<Trogin> i, std::unique_ptr<Trogerr> e): Being(g, n,
   std::move(o), std::move(i), std::move(e)),
   lastCommand(std::make_unique<Command>(game->getVocabulary(), "")) {

      types.push_back(ENTITY_PLAYER);
      setClass("player");

      setLongDescription(name + " is a player.");
   }

   /**************************************************************************/

   Player::Player(const Player &p, std::string n, std::unique_ptr<Trogout> o,
   std::unique_ptr<Trogin> i, std::unique_ptr<Trogerr> e): Being(p, n),
   lastCommand(std::make_unique<Command>(*(p.lastCommand))) {

      title = n;

      outStream = std::move(o);
      inStream = std::move(i);
      errStream = std::move(e);

      setLongDescription(name + " is a player.");
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
