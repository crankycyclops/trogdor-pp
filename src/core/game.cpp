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
#include "include/entities/item.h"
#include "include/entities/object.h"

#include "include/event/triggers/autoattack.h"
#include "include/event/triggers/deathdrop.h"
#include "include/event/triggers/respawn.h"

#include "include/iostream/trogout.h"


using namespace std;

namespace core {


   Game::Game(Trogout *e) {

      errStream = e;

      inGame = false;
      actions = new ActionMap(this);
      lastCommand = NULL;

      // initialize thread mutexes
      INIT_MUTEX(resourceMutex);
      INIT_MUTEX(timerMutex);

      timer = new Timer(this);
      events = new event::EventHandler;

      introduction.enabled           = DEFAULT_INTRODUCTION_ENABLED;
      introduction.pauseWhileReading = DEFAULT_INTRODUCTION_PAUSE;
      introduction.text              = "";
   }


   Game::~Game() {

      if (NULL != lastCommand) {
         delete lastCommand;
      }

      delete L;
      delete timer;
      delete events;
      delete parser;

      entities.destroyEntities();

      delete errStream;

      DESTROY_MUTEX(resourceMutex);
      DESTROY_MUTEX(timerMutex);

      delete actions;
   }


   void Game::initEntities() {

      entities = parser->getEntities();
      places = parser->getPlaces();
      things = parser->getThings();
      rooms = parser->getRooms();
      beings = parser->getBeings();
      creatures = parser->getCreatures();
      items = parser->getItems();
      objects = parser->getObjects();
   }


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
   void Game::initEvents() {

      eventListener->add("afterGotoLocation", new AutoAttackEventTrigger());
      eventListener->add("afterDie", new DeathDropEventTrigger());
      eventListener->add("afterDie", new RespawnEventTrigger());
   }


   bool Game::initialize(string gameXML) {

      try {
         parser = new Parser(this, gameXML);
         parser->parse();
      }

      catch (string error) {
         *errStream << error << endl;
         return false;
      }

      initEntities();
      initActions();

      defaultPlayer = parser->getDefaultPlayer();
      L = parser->getLuaState();
      eventListener = parser->getEventListener();

      initEvents();

      return true;
   }


   void Game::start() {

      MUTEX_LOCK(resourceMutex);
      inGame = true;
      timer->start();
      MUTEX_UNLOCK(resourceMutex);
   }


   void Game::stop() {

      MUTEX_LOCK(resourceMutex);
      timer->stop();
      inGame = false;
      MUTEX_UNLOCK(resourceMutex);
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

      Command *command = new Command();
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

            if (NULL != lastCommand) {
               delete command;
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
            MUTEX_LOCK(resourceMutex);
            action->execute(player, command, this);
            MUTEX_UNLOCK(resourceMutex);
         }
      }

      else {
         player->out() << "Sorry, I don't understand you." << endl;
      }

      if (lastCommand != command) {
         delete lastCommand;
      }

      lastCommand = command;
   }   
}

