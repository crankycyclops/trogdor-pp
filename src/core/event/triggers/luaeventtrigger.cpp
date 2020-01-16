#include <trogdor/event/triggers/luaeventtrigger.h>

namespace trogdor::event {


   void LuaEventTrigger::execute(EventArgumentList args) {

      L->call(function);

      for (unsigned int i = 0; i < args.size(); i++) {

         switch (args[i].index()) {

            case 0: // int
               L->pushArgument((double)std::get<int>(args[i]));
               break;

            case 1: // double
               L->pushArgument(std::get<double>(args[i]));
               break;

            case 2: // bool
               L->pushArgument(std::get<bool>(args[i]));
               break;

            case 3: // string
               L->pushArgument(std::get<std::string>(args[i]));
               break;

            // TODO: we should pass the Game once we have a Lua object for it
            case 4: // we ignore Game *
               break;

            default: // it's some kind of Entity

               entity::Entity *arg = std::get<entity::Entity *>(args[i]);

               if (nullptr == arg) {
                  L->pushNilArgument();
               }

               else {
                  L->pushArgument(arg);
               }

               break;
         }
      }

      L->execute(2);

      continueExecutionFlag = L->getBoolean(0);
      allowActionFlag = L->getBoolean(1);
   }
}
