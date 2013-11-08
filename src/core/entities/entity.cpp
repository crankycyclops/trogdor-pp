#include "../include/entities/entity.h"

#include "../include/game.h"
#include "../include/event/eventlistener.h"


using namespace std;

namespace core { namespace entity {


   // Special empty lists used for returning .end() iterator pairs in the case
   // of a non-existent list
   PlaceList     emptyPlaceList;
   RoomList      emptyRoomList;
   ThingList     emptyThingList;
   BeingList     emptyBeingList;
   ItemList      emptyItemList;
   PlayerList    emptyPlayerList;
   CreatureList  emptyCreatureList;
   ObjectList    emptyObjectList;


   Entity::Entity(Game *g, string n, Trogout *o, Trogin *i, Trogout *e) {

      // lame, but...legacy code, ya know?
      types.push_back(ENTITY_UNDEFINED);

      // this should always be overridden by a top-level Entity type
      className = "entity";

      game = g;
      name = n;
      outStream = o;
      errStream = e;
      inStream = i;

      // this will usually be set again later
      title = n;

      L = new LuaState();
      triggers = new event::EventListener();
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

      L = e.L;
      // TODO: we need to do some kind of intelligent copying for event handlers
      triggers = new event::EventListener();
   }

   /***************************************************************************/

   Entity::~Entity() {

      delete L;
      delete triggers;
      delete outStream;
      delete errStream;
      delete inStream;
   }

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
         game->addEventListener(triggers);
         game->addEventListener(observer->getEventListener());
         if (!game->event("beforeObserve", eventArgs)) {
            return;
         }
      }

      display(observer, displayFull);
      observedByMap[observer] = true;

      if (triggerEvents) {
         game->setupEventHandler();
         game->addEventListener(triggers);
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
         game->addEventListener(triggers);
         game->addEventListener(observer->getEventListener());
         if (!game->event("beforeGlance", eventArgs)) {
            return;
         }
      }

      displayShort(observer);
      glancedByMap[observer] = true;

      if (triggerEvents) {
         game->setupEventHandler();
         game->addEventListener(triggers);
         game->addEventListener(observer->getEventListener());
         game->event("afterGlance", eventArgs);
      }
   }
}}

