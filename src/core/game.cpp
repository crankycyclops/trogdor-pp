#include "include/game.h"
#include "include/actionmap.h"
#include "include/actions.h"
#include "include/timer/timer.h"
#include "include/parser/parser.h"

#include "include/entities/entity.h"
#include "include/entities/place.h"
#include "include/entities/room.h"
#include "include/entities/thing.h"
#include "include/entities/being.h"
#include "include/entities/player.h"
#include "include/entities/creature.h"
#include "include/entities/object.h"

#include "include/event/triggers/autoattack.h"
#include "include/event/triggers/deathdrop.h"
#include "include/event/triggers/respawn.h"

#include "include/iostream/nullout.h"
#include "include/iostream/nullin.h"
#include "include/iostream/trogout.h"


using namespace std;

namespace trogdor { namespace core {


   Game::Game(std::shared_ptr<Trogout> e) {

      errStream = e;

      inGame = false;
      actions = make_unique<ActionMap>(this);
      timer = make_unique<Timer>(this);
      events = make_unique<event::EventHandler>();

      introduction.enabled           = DEFAULT_INTRODUCTION_ENABLED;
      introduction.pauseWhileReading = DEFAULT_INTRODUCTION_PAUSE;
      introduction.text              = "";

      defaultPlayer = make_unique<entity::Player>(
         this, "default", new NullOut(), new NullIn(), new NullOut()
      );

      lastCommand = nullptr;
   }


   // Empty destructor required here, where parser.h has been included and the
   // parser class has been defined. If I don't include this, the compiler will
   // choke.
   Game::~Game() {}


   void Game::initActions() {

      actions->setAction("quit", new QuitAction);

      actions->setAction("fuck", new CussAction);
      setSynonym("shit", "fuck");
      setSynonym("bitch", "fuck");
      setSynonym("damn", "fuck");
      setSynonym("damnit", "fuck");
      setSynonym("asshole", "fuck");
      setSynonym("asshat", "fuck");

      actions->setAction("inv", new InventoryAction);
      setSynonym("inventory", "inv");
      setSynonym("list", "inv");

      actions->setAction("move", new MoveAction);
      setSynonym("go", "move");
      setSynonym("north", "move");
      setSynonym("south", "move");
      setSynonym("east", "move");
      setSynonym("west", "move");
      setSynonym("up", "move");
      setSynonym("down", "move");
      setSynonym("in", "move");
      setSynonym("out", "move");

      actions->setAction("look", new LookAction);
      setSynonym("observe", "look");
      setSynonym("see", "look");
      setSynonym("show", "look");
      setSynonym("describe", "look");
      setSynonym("examine", "look");

      actions->setAction("take", new TakeAction);
      setSynonym("acquire", "take");
      setSynonym("get", "take");
      setSynonym("grab", "take");
      setSynonym("own", "take");
      setSynonym("claim", "take");
      setSynonym("carry", "take");

      actions->setAction("drop", new DropAction);

      actions->setAction("attack", new AttackAction);
      setSynonym("hit", "attack");
      setSynonym("harm", "attack");
      setSynonym("kill", "attack");
      setSynonym("injure", "attack");
      setSynonym("maim", "attack");
      setSynonym("fight", "attack");
   }


   // NOTE: order is important!
   // TODO: does it make more sense for these events to be added via the trigger
   // objects themselves?
   void Game::initEvents() {

      eventListener->add("afterGotoLocation", new AutoAttackEventTrigger());
      eventListener->add("afterDie", new DeathDropEventTrigger());
      eventListener->add("afterDie", new RespawnEventTrigger());
   }


   bool Game::initialize(string gameXML) {

      try {
         parser = make_unique<Parser>(this, gameXML);
         parser->parse();
      }

      catch (string error) {
         *errStream << error << endl;
         return false;
      }

      initActions();

      L = parser->getLuaState();
      eventListener = parser->getEventListener();

      initEvents();

      return true;
   }


   void Game::start() {

      resourceMutex.lock();
      inGame = true;
      timer->start();
      resourceMutex.unlock();
   }


   void Game::stop() {

      resourceMutex.lock();
      timer->stop();
      inGame = false;
      resourceMutex.unlock();
   }


   Player *Game::createPlayer(string name, Trogout *outStream, Trogin *inStream,
   Trogout *errStream) {

      if (entities.isset(name)) {
         stringstream s;
         s << "Entity with name '" << name << "' already exists";
         throw s.str();
      }

      // clone the default player, giving it the specified name
      std::shared_ptr<Player> player = std::make_shared<Player>(
         *defaultPlayer, name, outStream, inStream, errStream
      );

      // if new player introduction is enabled, show it before inserting
      // the new player into the game
      // TODO: this works fine for a single player game, but in a
      // multi-player environment, the game can't stop every time a player
      // has to read the introduction...
      if (introduction.enabled && introduction.text.length() > 0) {

         if (introduction.pauseWhileReading) {
            stop();
         }

         // we really only need this to give player->in() something to do
         string blah;

         player->out() << introduction.text << endl << endl;
         player->out() << "Press enter to start." << endl;
         player->in() >> blah;
         player->out() << endl;

         if (introduction.pauseWhileReading) {
            start();
         }
      }

      entities.set(name, player);
      things.set(name, player);
      beings.set(name, player);
      players.set(name, player);

      // set Player's initial location
      player->setLocation(places.get("start"));
      places.get("start")->insertThing(player);

      // Player must see an initial description of where they are
      player->getLocation()->observe(player, false);

      return player.get();
   }


   void Game::insertTimerJob(TimerJob *j) {

      timer->insertJob(j);
   }


   void Game::removeTimerJob(TimerJob *j) {

      timer->removeJob(j);
   }


   unsigned long Game::getTime() const {

      return timer->getTime();
   }


   void Game::processCommand(Player *player) {

      std::shared_ptr<Command> command = make_shared<Command>();
      command->read(player);

      // do nothing if we're not in a running state
      if (!inGame) {
         player->out() << "Game is stopped and not accepting commands." << endl;
         return;
      }

      if (!command->isInvalid()) {

         // player is attempting to re-execute his last command
         if (0 == command->getVerb().compare("a") ||
         0 == command->getVerb().compare("again")) {

            if (nullptr != lastCommand) {
               command = lastCommand;
            }

            else {
               player->out() << "You haven't entered any commands yet!" << endl;
               return;
            }
         }

         string verb = command->getVerb();

         Action *action = actions->getAction(verb);

         // if the action doesn't exist, check if the verb is a synonym
         if (0 == action) {
            verb = getSynonym(verb);
            action = actions->getAction(verb);
         }

         if (0 == action || !action->checkSyntax(command)) {
            player->out() << "Sorry, I don't understand you." << endl;
         }

         else {
            resourceMutex.lock();
            action->execute(player, command, this);
            resourceMutex.unlock();
         }
      }

      else {
         player->out() << "Sorry, I don't understand you." << endl;
      }

      lastCommand = command;
   }   
}}

