#include "include/game.h"
#include "include/actionmap.h"
#include "include/actions.h"
#include "include/timer/timer.h"
#include "include/parser.h"

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


using namespace std;

namespace core {


   Game::Game() {

      // default input and output streams
      trogerr = &cerr;
      trogout = &cout;
      trogin = &cin;

      inGame = false;
      actions = new ActionMap(this);
      lastCommand = NULL;

      // initialize thread mutexes
      pthread_mutex_init(&resourceMutex, 0);
      pthread_mutex_init(&timerMutex, 0);

      timer = new Timer(this);
      events = new event::EventHandler;
   }


   Game::~Game() {

      if (NULL != lastCommand) {
         delete lastCommand;
      }

      delete timer;
      delete events;

      // TODO: delete all action objects initialized in initActions()
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

      QuitAction *quit = new QuitAction;
      actions->setAction("quit", quit);

      InventoryAction *inv = new InventoryAction;
      actions->setAction("inv", inv);
      actions->setAction("inventory", inv);
      actions->setAction("list", inv);

      MoveAction *move = new MoveAction;
      actions->setAction("move", move);
      actions->setAction("go", move);
      actions->setAction("north", move);
      actions->setAction("south", move);
      actions->setAction("east", move);
      actions->setAction("west", move);
      actions->setAction("up", move);
      actions->setAction("down", move);
      actions->setAction("in", move);
      actions->setAction("out", move);

      LookAction *look = new LookAction;
      actions->setAction("look", look);
      actions->setAction("observe", look);
      actions->setAction("see", look);
      actions->setAction("show", look);
      actions->setAction("describe", look);
      actions->setAction("examine", look);

      TakeAction *take = new TakeAction;
      actions->setAction("take", take);
      actions->setAction("acquire", take);
      actions->setAction("get", take);
      actions->setAction("grab", take);
      actions->setAction("own", take);
      actions->setAction("claim", take);
      actions->setAction("carry", take);

      DropAction *drop = new DropAction;
      actions->setAction("drop", drop);

      AttackAction *attack = new AttackAction;
      actions->setAction("attack", attack);
      actions->setAction("hit", attack);
      actions->setAction("harm", attack);
      actions->setAction("kill", attack);
      actions->setAction("injure", attack);
      actions->setAction("maim", attack);
      actions->setAction("fight", attack);
   }

   void Game::initEvents() {

      eventListener->add("afterGotoLocation", new AutoAttackEventTrigger());
   }


   bool Game::initialize(string gameXML) {

      try {
         parser = new Parser(this, gameXML);
         parser->parse();
      }

      catch (string error) {
         (*trogout) << error << endl;
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

      inGame = true;
      timer->start();
   }


   void Game::stop() {

      // TODO: cleanup?
      timer->stop();
      inGame = false;
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
      command->read(*trogin, *trogout);

      if (!command->isInvalid()) {

         // player is attempting to re-execute his last command
         if (0 == command->getVerb().compare("a") ||
         0 == command->getVerb().compare("again")) {

            if (NULL != lastCommand) {
               delete command;
               command = lastCommand;
            }

            else {
               *trogout << "You haven't entered any commands yet!" << endl;
               return;
            }
         }

         Action *action = actions->getAction(command->getVerb());

         if (0 == action || !action->checkSyntax(command)) {
            *trogout << "Sorry, I don't understand you." << endl;
         }

         else {

            pthread_mutex_lock(&resourceMutex);
            action->execute(player, command, this);
            pthread_mutex_unlock(&resourceMutex);
         }
      }

      else {
         *trogout << "Sorry, I don't understand you." << endl;
      }

      if (lastCommand != command) {
         delete lastCommand;
      }

      lastCommand = command;
   }   
}

