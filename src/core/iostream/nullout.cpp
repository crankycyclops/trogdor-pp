#include "../include/iostream/nullout.h"


using namespace std;

namespace core {


   void NullOut::flush() {

      return; // complicated, huh... :)
   }

   Trogout *NullOut::clone() {

      return new NullOut;
   }
}

