#include <trogdor/event/eventtrigger.h>

#include <trogdor/event/triggers/autoattack.h>
#include <trogdor/event/triggers/deathdrop.h>
#include <trogdor/event/triggers/respawn.h>
#include <trogdor/event/triggers/luaeventtrigger.h>

namespace trogdor::event {


   std::unordered_map<std::string_view, std::type_info *> EventTrigger::types;

   /**************************************************************************/

   // See note in the comment above the destructor definition in
   // include/event/eventtrigger.h for an explanation of why this is here.
   EventTrigger::~EventTrigger() {}

   /**************************************************************************/

   void EventTrigger::initBuiltinTypes() {

      types[AutoAttackEventTrigger::CLASS_NAME] = const_cast<std::type_info *>(&typeid(AutoAttackEventTrigger));
      types[RespawnEventTrigger::CLASS_NAME] = const_cast<std::type_info *>(&typeid(RespawnEventTrigger));
      types[DeathDropEventTrigger::CLASS_NAME] = const_cast<std::type_info *>(&typeid(DeathDropEventTrigger));
      types[LuaEventTrigger::CLASS_NAME] = const_cast<std::type_info *>(&typeid(LuaEventTrigger));
   }

   /**************************************************************************/

   std::shared_ptr<serial::Serializable> EventTrigger::serialize() {

      std::shared_ptr<serial::Serializable> data = std::make_shared<serial::Serializable>();

      data->set("type", getClassName());
      return data;
   }
}
