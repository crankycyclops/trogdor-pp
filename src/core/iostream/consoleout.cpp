#include "../include/iostream/consoleout.h"


using namespace std;

namespace core {


   virtual void ConsoleOut::flush() {

      cout << getBufferStr();
      cout.flush();
   }
}

