#include <Arduino.h>
extern "C" {
#include "networkhandler.h"
#include "opener_error.h"
}
#include "opener_mbed_socket_compat.h"

EipStatus NetworkHandlerInitializePlatform(void) {
  return opener_get_network() ? kEipStatusOk : kEipStatusError;
}

void ShutdownSocketPlatform(int socket_handle) { shutdown(socket_handle, SHUT_RDWR); }
void CloseSocketPlatform(int socket_handle) { close(socket_handle); }
int SetSocketToNonBlocking(int socket_handle) { return fcntl(socket_handle, F_SETFL, O_NONBLOCK); }
MicroSeconds GetMicroSeconds(void) { return (MicroSeconds)micros(); }
MilliSeconds GetMilliSeconds(void) { return (MilliSeconds)millis(); }
int SetQosOnSocket(const int socket, CipUsint qos_value) {
  int tos = ((int)qos_value) << 2;
  return setsockopt(socket, IPPROTO_IP, IP_TOS, &tos, sizeof(tos));
}
