#include "../include/iostream/nullout.h"


using namespace std;

namespace trogdor {


   void NullOut::flush() {

      return; // complicated, huh... :)
   }

   std::unique_ptr<Trogout> NullOut::clone() {

      return std::make_unique<NullOut>();
   }
}

