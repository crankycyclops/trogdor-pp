#ifndef TCPIN_H
#define TCPIN_H


#include "../network/tcpconnection.h"
#include "../../../../core/include/iostream/trogin.h"


using namespace std;


/*
 Input "stream" that uses data from a TCP connection's read buffer.
*/
class TCPIn: public core::Trogin {

   private:

      TCPConnection::ptr connection;

   public:

      inline TCPIn(TCPConnection::ptr c) {connection = c;}

      // Allows us to use a new TCP connection object in the event that a
      // previous connection expires.
      inline void updateConnection(TCPConnection::ptr c) {connection = c;}

      /*
       See include/iostream/trogin.h for details.
      */
      virtual core::Trogin *clone();

      // For now, I only need to define input for strings.
      virtual core::Trogin &operator>> (string &val);
};


#endif

