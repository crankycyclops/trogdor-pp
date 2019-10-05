#include "../../include/event/triggers/luaeventtrigger.h"

using namespace std;

namespace trogdor { namespace event {


   void LuaEventTrigger::execute(EventArgumentList args) {

      L->call(function);

      for (int i = 0; i < args.size(); i++) {

         switch (args[i].which()) {

            case 0: // int
               L->pushArgument((double)boost::get<int>(args[i]));
               break;

            case 1: // double
               L->pushArgument(boost::get<double>(args[i]));
               break;

            case 2: // bool
               L->pushArgument(boost::get<bool>(args[i]));
               break;

            case 3: // string
               L->pushArgument(boost::get<string>(args[i]));
               break;

            // TODO: we should pass the Game once we have a Lua object for it
            case 4: // we ignore Game *
               break;

            default: // it's some kind of Entity

               entity::Entity *arg = boost::get<entity::Entity *>(args[i]);

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
}}

