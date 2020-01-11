#include <trogdor/iostream/nullout.h>


namespace trogdor {


   void NullOut::flush() {

      return; // complicated, huh... :)
   }

   std::unique_ptr<Trogout> NullOut::clone() {

      return std::make_unique<NullOut>();
   }
}
