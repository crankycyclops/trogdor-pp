#include "../include/iostream/consoleout.h"


using namespace std;

namespace core {


   void ConsoleOut::flush() {

      cout << getBufferStr();
      cout.flush();
   }

   Trogout *ConsoleOut::clone() {

      return new ConsoleOut();
   }
}

