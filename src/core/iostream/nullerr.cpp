#include <trogdor/iostream/nullerr.h>


namespace trogdor {


   void NullErr::flush() {

      return;
   }

   std::unique_ptr<Trogerr> NullErr::copy() {

      return std::make_unique<NullErr>();
   }
}
