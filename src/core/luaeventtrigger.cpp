#include "include/luaeventtrigger.h"

using namespace std;

namespace core { namespace event {


   void LuaEventTrigger::execute(EventArgumentList args) {

      L->call(function);

      // TODO: check the order we push and make sure the order we add args is
      // the order in which they appear in the Lua argument list
      for (int i = args.size() - 1; i >= 0; i--) {

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

            default: // it's some kind of Entity

               Entity *arg = boost::get<Entity *>(args[i]);

               if (0 == arg) {
                  L->pushNilArgument();
               }

               else {
                  L->pushArgument(arg->getLuaTable());
               }

               break;
         }
      }

      L->execute();

      // TODO: get return values (have to implement this in LuaState first) and
      // set allowActionFlag and continueHandlersFlag.
   }
}}

