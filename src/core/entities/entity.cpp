#include <trogdor/entities/entity.h>

#include <trogdor/game.h>
#include <trogdor/event/eventlistener.h>
#include <trogdor/exception/validationexception.h>


namespace trogdor { namespace entity {


   // Special empty lists used for returning .end() iterator pairs in the case
   // of a non-existent list
   PlaceList     emptyPlaceList;
   RoomList      emptyRoomList;
   ThingList     emptyThingList;
   BeingList     emptyBeingList;
   PlayerList    emptyPlayerList;
   CreatureList  emptyCreatureList;
   ObjectList    emptyObjectList;

   /***************************************************************************/

   // The title property will usually be set to something more descriptive later
   Entity::Entity(Game *g, std::string n, std::unique_ptr<Trogout> o,
   std::unique_ptr<Trogin> i, std::unique_ptr<Trogerr> e): game(g), name(n),
   title(n), outStream(std::move(o)), errStream(std::move(e)), inStream(std::move(i)) {

      if (!isNameValid(n)) {
         throw ValidationException(std::string("name '") + n
            + "' is invalid (must contain only letters, numbers, underscores, "
            + "and dashes.)");
      }

      types.push_back(ENTITY_ENTITY);

      // this should always be overridden by a top-level Entity type
      className = "entity";

      L = std::make_shared<LuaState>(g);
      triggers = std::make_unique<event::EventListener>();
   }

   /***************************************************************************/

   Entity::Entity(const Entity &e, std::string n) {

      if (!isNameValid(n)) {
         throw ValidationException(std::string("name '") + n
            + "' is invalid (must contain only letters, numbers, underscores, "
            "and dashes.)");
      }

      name = n;

      types = e.types;
      className = e.className;
      game = e.game;
      title = e.title;
      longDesc = e.longDesc;
      shortDesc = e.shortDesc;
      msgs = e.msgs;
      tags = e.tags;
      outStream = e.outStream->clone();
      errStream = e.errStream->copy();
      inStream = e.inStream->clone();

      L = std::make_shared<LuaState>(*e.L);
      // TODO: we need to do some kind of intelligent copying for event handlers
      triggers = std::make_unique<event::EventListener>();
   }

   /***************************************************************************/

   // For some reason that mystifies me, this is required to compile successfully
   Entity::~Entity() {}

   /***************************************************************************/

   void Entity::setTag(std::string tag) {

      tags.insert(tag);
   }

   /***************************************************************************/

   void Entity::removeTag(std::string tag) {

      if (tags.end() != tags.find(tag)) {
         tags.erase(tag);
      }
   }

   /***************************************************************************/

   void Entity::setGame(Game *g) {

      game = g;
   }

   /***************************************************************************/

   void Entity::display(Being *observer, bool displayFull) {

      if (!observedBy(observer) || displayFull) {
         if (ENTITY_PLAYER == observer->getType()) {
            observer->out("display") << getLongDescription() << std::endl;
         }
      }

      else {
         displayShort(observer);
      }

      observedByMap.insert(observer);
   }

   /***************************************************************************/

   void Entity::displayShort(Being *observer) {

      if (ENTITY_PLAYER == observer->getType()
      && getShortDescription().length() > 0) {
         observer->out("display") << getShortDescription() << std::endl;
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
      observedByMap.insert(observer);

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
      glancedByMap.insert(observer);

      if (triggerEvents) {
         game->setupEventHandler();
         game->addEventListener(triggers.get());
         game->addEventListener(observer->getEventListener());
         game->event("afterGlance", eventArgs);
      }
   }
}}
