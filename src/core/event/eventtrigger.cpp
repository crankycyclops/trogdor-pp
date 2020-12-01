#include <trogdor/event/eventtrigger.h>

#include <trogdor/event/triggers/autoattack.h>
#include <trogdor/event/triggers/deathdrop.h>
#include <trogdor/event/triggers/respawn.h>
#include <trogdor/event/triggers/luaeventtrigger.h>

namespace trogdor::event {


   std::unordered_map<std::string_view, std::type_info *> EventTrigger::types;

   std::unordered_map<
      std::type_index,
      std::function<std::unique_ptr<EventTrigger>(std::any)>
   > EventTrigger::instantiators;

   /**************************************************************************/

   // See note in the comment above the destructor definition in
   // include/event/eventtrigger.h for an explanation of why this is here.
   EventTrigger::~EventTrigger() {}

   /**************************************************************************/

   void EventTrigger::initBuiltinTypes() {

      AutoAttackEventTrigger::init();
      DeathDropEventTrigger::init();
      RespawnEventTrigger::init();
      LuaEventTrigger::init();
   }

   /**************************************************************************/

   std::shared_ptr<serial::Serializable> EventTrigger::serialize() {

      std::shared_ptr<serial::Serializable> data = std::make_shared<serial::Serializable>();

      data->set("type", getClassName());
      return data;
   }
}
