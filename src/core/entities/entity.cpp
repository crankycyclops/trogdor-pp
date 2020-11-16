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
   std::unique_ptr<Trogerr> e): game(g), name(n), outStream(std::move(o)),
   errStream(std::move(e)) {

      if (!isNameValid(n)) {
         throw ValidationException(std::string("name '") + n
            + "' is invalid (must contain only letters, numbers, underscores, "
            + "dashes, and single spaces.)");
      }

      types.push_back(ENTITY_ENTITY);

      // this should always be overridden by a top-level Entity type
      className = "entity";

      // Default value for the Entity's title is its name
      setProperty("title", n);

      setPropertyValidator(TitleProperty, [&](PropertyValue v) -> int {return isPropertyValueString(v);});
      setPropertyValidator(LongDescProperty, [&](PropertyValue v) -> int {return isPropertyValueString(v);});
      setPropertyValidator(ShortDescProperty, [&](PropertyValue v) -> int {return isPropertyValueString(v);});

      L = std::make_shared<LuaState>(g);
      triggers = std::make_unique<event::EventListener>();
   }

   /***************************************************************************/

   Entity::Entity(const Entity &e, std::string n): msgs(e.msgs), tags(e.tags),
   properties(e.properties), propertyValidators(e.propertyValidators),
   types(e.types), game(nullptr), name(n), className(e.className) {

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
      // Will have to also install property validators here, so I'll have to
      // shunt those off to a private method that gets called here and in the
      // default constructor. Do this for all entities that implement property
      // validators.
   }

   /***************************************************************************/

   std::shared_ptr<serial::Serializable> Entity::serialize() {

      std::shared_ptr<serial::Serializable> data = std::make_shared<serial::Serializable>();
      std::shared_ptr<serial::Serializable> serializedMsgs = std::make_shared<serial::Serializable>();
      std::shared_ptr<serial::Serializable> serializedMeta = std::make_shared<serial::Serializable>();
      std::shared_ptr<serial::Serializable> serializedProperties = std::make_shared<serial::Serializable>();

      data->set("name", name);
      data->set("class", className);

      std::vector<std::string> typeStrs;

      for (const auto &type: types) {
         typeStrs.push_back(typeToStr(type));
      }

      data->set("types", typeStrs);

      std::vector<std::string> tagsArray;

      for (auto it = msgs.cbegin(); it != msgs.cend(); it++) {
         serializedMsgs->set(it->first, it->second);
      }

      data->set("messages", serializedMsgs);

      for (const auto &tag: tags) {
         tagsArray.push_back(tag);
      }

      data->set("tags", tagsArray);

      for (const auto &metaItem: meta) {
         serializedMeta->set(metaItem.first, metaItem.second);
      }

      data->set("meta", serializedMeta);

      for (const auto &property: properties) {
         std::visit([&](auto &&value) {serializedProperties->set(property.first, value);}, property.second);
      }

      data->set("properties", serializedProperties);
      data->set("lua", L->serialize());

      // TODO: I still need to figure out how to serialize event triggers (I
      // think each trigger will also have a serialize() method.) In that case,
      // EventTrigger will also have to have a pure virtual method that each
      // kind of trigger implements, just like we do with the Entity hierarchy.
      return data;
   }

   /***************************************************************************/

   void Entity::executeCallback(std::string operation, std::any data) {

      std::vector<std::shared_ptr<EntityCallback> *> toRemove;

      if (callbacks.end() != callbacks.find(operation)) {
         for (auto &callback: callbacks[operation]) {
            if ((*callback)(data)) {
               toRemove.push_back(&callback);
            }
         }
      }

      // If a callback flagged itself for removal, do so now
      while (toRemove.size()) {
         removeCallback(operation, *toRemove.back());
         toRemove.pop_back();
      }
   }

   /***************************************************************************/

   void Entity::addCallback(
      std::string operation,
      std::shared_ptr<EntityCallback> callback
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
      const std::shared_ptr<EntityCallback> &callback
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

      mutex.unlock();
      executeCallback("setTag", std::tuple<std::string, Entity *>({tag, this}));
   }

   /***************************************************************************/

   void Entity::removeTag(std::string tag) {

      mutex.lock();

      if (tags.end() != tags.find(tag)) {
         tags.erase(tag);
      }

      mutex.unlock();
      executeCallback("removeTag", std::tuple<std::string, Entity *>({tag, this}));
   }

   /***************************************************************************/

   void Entity::setGame(Game *g) {

      mutex.lock();
      game = g;
      mutex.unlock();
   }

   /***************************************************************************/

   void Entity::displayShort(Being *observer) {

      if (
         ENTITY_PLAYER == observer->getType() &&
         isPropertySet(ShortDescProperty) &&
         getProperty<std::string>(ShortDescProperty).length() > 0
      ) {
         observer->out("display") << getProperty<std::string>(ShortDescProperty) << std::endl;
      }
   }
}
