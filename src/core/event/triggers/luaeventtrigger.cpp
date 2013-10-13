#include "../../include/event/triggers/luaeventtrigger.h"

using namespace std;

namespace core { namespace event {


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

            case 4: // we ignore Game *
               break;

            default: // it's some kind of Entity

               Entity *arg = boost::get<Entity *>(args[i]);

               if (0 == arg) {
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

