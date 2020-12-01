#include <trogdor/event/triggers/luaeventtrigger.h>
#include <trogdor/exception/undefinedexception.h>

namespace trogdor::event {


   LuaEventTrigger::LuaEventTrigger(const serial::Serializable &data, Trogerr &e):
   errStream(e) {

      // TODO
   }

   /**************************************************************************/

   const char *LuaEventTrigger::getClassName() {

      return CLASS_NAME;
   }

   /**************************************************************************/

   void LuaEventTrigger::init() {

      registerType(
         CLASS_NAME,
         const_cast<std::type_info *>(&typeid(LuaEventTrigger)),
         [] (std::any arg) -> std::unique_ptr<EventTrigger> {

            // Invoke the copy constructor
            if (typeid(LuaEventTrigger) == arg.type()) {
               return std::make_unique<LuaEventTrigger>(std::any_cast<LuaEventTrigger &>(arg));
            }

            // Invoke the deserialization constructor
            else if (typeid(std::tuple<serial::Serializable, Trogerr *>) == arg.type()) {
               auto args = std::any_cast<std::tuple<serial::Serializable, Trogerr *> &>(arg);
               return std::make_unique<LuaEventTrigger>(std::get<0>(args), *std::get<1>(args));
            }

            else {
               throw UndefinedException("Unsupported argument type in LuaEventTrigger instantiator");
            }
         }
      );
   }

   /**************************************************************************/

   EventReturn LuaEventTrigger::operator()(Event e) {

      try {

         L->lock();
         L->call(function);

         for (auto const &argument: e.getArguments()) {

            switch (argument.index()) {

               case 0: // int
                  L->pushArgument((double)std::get<int>(argument));
                  break;

               case 1: // double
                  L->pushArgument(std::get<double>(argument));
                  break;

               case 2: // bool
                  L->pushArgument(std::get<bool>(argument));
                  break;

               case 3: // string
                  L->pushArgument(std::get<std::string>(argument));
                  break;

               // TODO: we should pass the Game once we have a Lua object for it
               case 4: // we ignore Game *
                  break;

               default: // it's some kind of Entity

                  entity::Entity *e = std::get<entity::Entity *>(argument);

                  if (nullptr == e) {
                     L->pushNilArgument();
                  }

                  else {
                     L->pushArgument(e);
                  }

                  break;
            }
         }

         L->execute(2);
         EventReturn retVals = {L->getBoolean(1), L->getBoolean(0)};

         L->unlock();
         return retVals;
      }

      catch (const LuaException &e) {

         L->unlock();
         errStream << e.what() << std::endl;
         return {true, true};
      }
   }

   /**************************************************************************/

   std::shared_ptr<serial::Serializable> LuaEventTrigger::serialize() {

      std::shared_ptr<serial::Serializable> data = EventTrigger::serialize();

      data->set("function", function);
      data->set("lua", L->serialize());

      return data;
   }
}
