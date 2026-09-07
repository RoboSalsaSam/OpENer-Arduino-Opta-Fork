//networkhandler.h was simplified for the Arduino library and wrapped with extern "C" so the OpENer C code 
//can interface correctly with the C++ Mbed implementation.

#ifndef OPTA_NETWORKHANDLER_H_
#define OPTA_NETWORKHANDLER_H_
#include "typedefs.h"
#define OPENER_SOCKET_WOULD_BLOCK EWOULDBLOCK
#ifdef __cplusplus
extern "C" {
#endif
EipStatus NetworkHandlerInitializePlatform(void);
void ShutdownSocketPlatform(int socket_handle);
void CloseSocketPlatform(int socket_handle);
int SetSocketToNonBlocking(int socket_handle);
MicroSeconds GetMicroSeconds(void);
MilliSeconds GetMilliSeconds(void);
int SetQosOnSocket(const int socket, CipUsint qos_value);
#ifdef __cplusplus
}
#endif
#endif
