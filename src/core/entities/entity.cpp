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


   Entity::Entity(Game *g, Trogout *o, string n) {

      // not sure if this will ever actually be used, but meh...
      type = ENTITY_UNDEFINED;

      game = g;
      name = n;
      outStream = o;

      // this will usually be set again later
      title = n;

      L = new LuaState();
      triggers = new event::EventListener();
   }

   /***************************************************************************/

   Entity::Entity(const Entity &e, string n) {

      name = n;

      type = e.type;
      game = e.game;
      title = e.title;
      longDesc = e.longDesc;
      shortDesc = e.shortDesc;
      msgs = e.msgs;
      outStream = e.outStream->clone();

      // TODO: we need to do some kind of intelligent copying here, so that we
      // can retain all parsed scripts, event handlers, etc.
      L = new LuaState();
      triggers = new event::EventListener();
   }

   /***************************************************************************/

   Entity::~Entity() {

      delete L;
      delete triggers;
      delete outStream;
   }

   /***************************************************************************/

   LuaTable *Entity::getLuaTable() const {

      LuaTable *table = new LuaTable();

      table->setField("type", getTypeName());
      table->setField("name", name);
      table->setField("title", title);
      table->setField("longdesc", longDesc);
      table->setField("shortdesc", shortDesc);

      return table;
   }

   /***************************************************************************/

   void Entity::display(Being *observer, bool displayFull) {

      if (!observedBy(observer) || displayFull) {
         if (ENTITY_PLAYER == observer->getType()) {
            *game->trogout << getLongDescription() << endl;
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
         *game->trogout << getShortDescription() << endl;
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

