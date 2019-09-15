#include "../include/entities/entity.h"

#include "../include/game.h"
#include "../include/event/eventlistener.h"


using namespace std;

namespace trogdor { namespace core { namespace entity {


   // Special empty lists used for returning .end() iterator pairs in the case
   // of a non-existent list
   PlaceList     emptyPlaceList;
   RoomList      emptyRoomList;
   ThingList     emptyThingList;
   BeingList     emptyBeingList;
   PlayerList    emptyPlayerList;
   CreatureList  emptyCreatureList;
   ObjectList    emptyObjectList;


   // The title property will usually be set to something more descriptive later
   Entity::Entity(Game *g, string n, std::unique_ptr<Trogout> o,
   std::unique_ptr<Trogin> i, std::unique_ptr<Trogout> e): game(g), name(n),
   outStream(std::move(o)), errStream(std::move(e)), inStream(std::move(i)), title(n) {

      types.push_back(ENTITY_ENTITY);

      // this should always be overridden by a top-level Entity type
      className = "entity";

      L = std::make_shared<LuaState>();
      triggers = std::make_unique<event::EventListener>();
   }

   /***************************************************************************/

   Entity::Entity(const Entity &e, string n) {

      name = n;

      types = e.types;
      className = e.className;
      game = e.game;
      title = e.title;
      longDesc = e.longDesc;
      shortDesc = e.shortDesc;
      msgs = e.msgs;
      outStream = e.outStream->clone();
      errStream = e.errStream->clone();
      inStream = e.inStream->clone();

      L = make_shared<LuaState>(*e.L);
      // TODO: we need to do some kind of intelligent copying for event handlers
      triggers = std::make_unique<event::EventListener>();
   }

   /***************************************************************************/

   // For some reason that mystifies me, this is required to compile successfully
   Entity::~Entity() {}

   /***************************************************************************/

   void Entity::display(Being *observer, bool displayFull) {

      if (!observedBy(observer) || displayFull) {
         if (ENTITY_PLAYER == observer->getType()) {
            observer->out("display") << getLongDescription() << endl;
         }
      }

      else {
         displayShort(observer);
      }

      observedByMap[observer] = true;
   }

   /***************************************************************************/

   void Entity::displayShort(Being *observer) {

      if (ENTITY_PLAYER == observer->getType()
      && getShortDescription().length() > 0) {
         observer->out("display") << getShortDescription() << endl;
      }
   }

   /***************************************************************************/

   void Entity::observe(Being *observer, bool triggerEvents, bool displayFull) {

      EventArgumentList eventArgs;

      if (triggerEvents) {

         eventArgs.push_back(this);
         eventArgs.push_back(observer);

         game->setupEventHandler();
         game->addEventListener(triggers.get());
         game->addEventListener(observer->getEventListener());
         if (!game->event("beforeObserve", eventArgs)) {
            return;
         }
      }

      display(observer, displayFull);
      observedByMap[observer] = true;

      if (triggerEvents) {
         game->setupEventHandler();
         game->addEventListener(triggers.get());
         game->addEventListener(observer->getEventListener());
         game->event("afterObserve", eventArgs);
      }
   }

   /***************************************************************************/

   void Entity::glance(Being *observer, bool triggerEvents) {

      EventArgumentList eventArgs;

      if (triggerEvents) {

         eventArgs.push_back(this);
         eventArgs.push_back(observer);

         game->setupEventHandler();
         game->addEventListener(triggers.get());
         game->addEventListener(observer->getEventListener());
         if (!game->event("beforeGlance", eventArgs)) {
            return;
         }
      }

      displayShort(observer);
      glancedByMap[observer] = true;

      if (triggerEvents) {
         game->setupEventHandler();
         game->addEventListener(triggers.get());
         game->addEventListener(observer->getEventListener());
         game->event("afterGlance", eventArgs);
      }
   }
}}}

