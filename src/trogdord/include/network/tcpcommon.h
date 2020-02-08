#ifndef TCPCOMMON_H
#define TCPCOMMON_H


// Response sent to incoming connections to let them know trogdord is ready to
// send and receive commands
#define READY ("READY")

// Signals the end of a transmission
#define EOT ('\0')


#endif
