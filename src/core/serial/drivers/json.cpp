#include <trogdor/serial/serializable.h>
#include <trogdor/serial/drivers/json.h>

namespace trogdor::serial {


   std::any Json::serialize(const Serializable &data) {

      // TODO
      return "{}";
   }

   /************************************************************************/

   Serializable Json::deserialize(const std::any &data) {

      // TODO
      return Serializable();
   }
}
