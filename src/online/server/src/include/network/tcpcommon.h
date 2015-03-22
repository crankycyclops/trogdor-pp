#ifndef TCPCOMMON_H
#define TCPCOMMON_H


#define EOT 0x4 // EOT (end of transmission) character

// Callback that takes a single argument and is called at the
// conclusion of an asynchronous event. 
class TCPConnection;
typedef void (*callback_t)(TCPConnection *, void *);


#endif

