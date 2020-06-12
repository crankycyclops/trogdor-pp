#include <trogdor/event/triggers/luaeventtrigger.h>

namespace trogdor::event {


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
}
