#include <trogdor/entities/entity.h>
#include <trogdor/entities/being.h>

#include <trogdor/game.h>
#include <trogdor/event/eventlistener.h>
#include <trogdor/event/triggers/luaeventtrigger.h>
#include <trogdor/exception/validationexception.h>


namespace trogdor::entity {


   // Special empty lists used for returning results when no result exists
   ResourceList  emptyResourceList;
   TangibleList  emptyTangibleList;
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
   std::unique_ptr<Trogerr> e): game(g), name(n), title(n), outStream(std::move(o)),
   errStream(std::move(e)) {

      if (!isNameValid(n)) {
         throw ValidationException(std::string("name '") + n
            + "' is invalid (must contain only letters, numbers, underscores, "
            + "dashes, and single spaces.)");
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

      L = std::make_shared<LuaState>(*e.L);
      triggers = std::make_unique<event::EventListener>(*e.triggers);

      for (const auto &event: triggers->getTriggers()) {

         for (auto &trigger: event.second) {

            if (trigger) {

               // Using this instead of typeid(*trigger) satisfies te compiler
               // gods and fixes the following warning: expression with side
               // effects will be evaluated despite being used as an operand to
               // 'typeid'
               auto &t = *trigger.get();

               if (typeid(event::LuaEventTrigger) == typeid(t)) {
                  dynamic_cast<event::LuaEventTrigger *>(trigger.get())->setLuaState(L);
               }
            }
         }
      }
   }

   /***************************************************************************/

   Entity::~Entity() {}

   /***************************************************************************/

   Entity::Entity(const serial::Serializable &data) {

      // TODO
   }

   /***************************************************************************/

   serial::Serializable Entity::serialize() {

      serial::Serializable data;

      // TODO
      return data;
   }

   /***************************************************************************/

   void Entity::addCallback(
      std::string operation,
      std::shared_ptr<std::function<void(std::any)>> callback
   ) {

      mutex.lock();
      callbacks[operation].push_back(callback);
      mutex.unlock();
   }

   /***************************************************************************/

   size_t Entity::removeCallbacks(std::string operation) {

      mutex.lock();

      if (callbacks.end() != callbacks.find(operation)) {

         size_t size = callbacks[operation].size();

         callbacks.erase(operation);
         return size;
      }

      mutex.unlock();
      return 0;
   }

   /***************************************************************************/

   void Entity::removeCallback(
      std::string operation,
      const std::shared_ptr<std::function<void(std::any)>> &callback
   ) {

      mutex.lock();

      if (callbacks.end() != callbacks.find(operation)) {

         auto it = std::find(callbacks[operation].begin(), callbacks[operation].end(), callback);

         if (it != callbacks[operation].end()) {
            callbacks[operation].erase(it);
         }
      }

      mutex.unlock();
   }

   /***************************************************************************/

   void Entity::setTag(std::string tag) {

      mutex.lock();

      // Make sure the tag only gets inserted once
      if (tags.end() == tags.find(tag)) {
         tags.insert(tag);
      }

      if (callbacks.end() != callbacks.find("setTag")) {

         std::tuple<std::string, Entity *> args = {tag, this};

         for (const auto &callback: callbacks["setTag"]) {
            (*callback)(args);
         }
      }

      mutex.unlock();
   }

   /***************************************************************************/

   void Entity::removeTag(std::string tag) {

      mutex.lock();

      if (tags.end() != tags.find(tag)) {
         tags.erase(tag);
      }

      if (callbacks.end() != callbacks.find("removeTag")) {

         std::tuple<std::string, Entity *> args = {tag, this};

         for (const auto &callback: callbacks["removeTag"]) {
            (*callback)(args);
         }
      }

      mutex.unlock();
   }

   /***************************************************************************/

   void Entity::setGame(Game *g) {

      mutex.lock();
      game = g;
      mutex.unlock();
   }

   /***************************************************************************/

   void Entity::displayShort(Being *observer) {

      if (ENTITY_PLAYER == observer->getType()
      && getShortDescription().length() > 0) {
         observer->out("display") << getShortDescription() << std::endl;
      }
   }
}
