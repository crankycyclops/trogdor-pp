#include <trogdor/entities/entity.h>

#include <trogdor/game.h>
#include <trogdor/event/eventlistener.h>
#include <trogdor/exception/validationexception.h>


namespace trogdor::entity {


   // If no output channel is specified when output is sent to the Entity's
   // output stream, this is the channel we use
   const char *Entity::DEFAULT_OUTPUT_CHANNEL = "notifications";

   // Special empty lists used for returning results when no result exists
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

   Entity::Entity(const Entity &e, std::string n): msgs(e.msgs), tags(e.tags),
   types(e.types), game(nullptr), name(n), className(e.className), title(e.title),
   longDesc(e.longDesc), shortDesc(e.shortDesc) {

      if (!isNameValid(n)) {
         throw ValidationException(std::string("name '") + n
            + "' is invalid (must contain only letters, numbers, underscores, "
            "and dashes.)");
      }

      outStream = e.outStream->clone();
      errStream = e.errStream->copy();
      inStream = e.inStream->clone();

      L = std::make_shared<LuaState>(*e.L);
      // TODO: how do I pass in L when I create a fresh LuaEventTrigger in the
      //EventListener's copy constructor?
      triggers = std::make_unique<event::EventListener>(*e.triggers);
   }

   /***************************************************************************/

   Entity::~Entity() {}

   /***************************************************************************/

   void Entity::setTag(std::string tag) {

      // Make sure the tag only gets inserted once
      if (tags.end() == tags.find(tag)) {
         tags.insert(tag);
      }
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

      if (triggerEvents && !game->event({
         "beforeObserve",
         {triggers.get(), observer->getEventListener()},
         {this, observer}
      })) {
         return;
      }

      display(observer, displayFull);
      observedByMap.insert(observer);

      if (triggerEvents) {
         game->event({
            "afterObserve",
            {triggers.get(), observer->getEventListener()},
            {this, observer}
         });
      }
   }

   /***************************************************************************/

   void Entity::glance(Being *observer, bool triggerEvents) {

      if (triggerEvents && !game->event({
         "beforeGlance",
         {triggers.get(), observer->getEventListener()},
         {this, observer}
      })) {
         return;
      }

      displayShort(observer);
      glancedByMap.insert(observer);

      if (triggerEvents) {
         game->event({
            "afterGlance",
            {triggers.get(), observer->getEventListener()},
            {this, observer}
         });
      }
   }
}
