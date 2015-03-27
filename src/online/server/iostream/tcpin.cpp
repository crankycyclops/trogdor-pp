#include "../include/iostream/tcpin.h"


using namespace std;


core::Trogin &TCPIn::operator>> (string &val) {

   val = connection->isOpen() ? connection->getBufferStr() : "";
}

core::Trogin *TCPIn::clone() {

   return new TCPIn(connection);
}

