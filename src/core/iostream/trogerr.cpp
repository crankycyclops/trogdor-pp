#include <trogdor/iostream/trogerr.h>
#include <trogdor/exception/undefinedexception.h>


namespace trogdor {


   Trogerr::~Trogerr() {}

   // DO NOT USE THIS! See trogerr.h for an explanation of why.
   std::unique_ptr<Trogout> Trogerr::clone() {

      throw UndefinedException("Do not use Trogerr::clone()! See trogerr.h for an explanation.");
   }
}
