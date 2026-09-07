//networkconfig.h provides the Opta-specific network configuration interface used by networkconfig.cpp, 
//replacing the original platform-specific network configuration definition.

#ifndef OPTA_NETWORKCONFIG_H_
#define OPTA_NETWORKCONFIG_H_
#include "opener_api.h"
#ifdef __cplusplus
extern "C" {
#endif
EipStatus IfaceGetMacAddress(TcpIpInterface *, uint8_t *const physical_address);
EipStatus IfaceGetConfiguration(TcpIpInterface *, CipTcpIpInterfaceConfiguration *cfg);
void GetHostName(CipString *hostname);
EipStatus IfaceWaitForIp(TcpIpInterface *const iface, int timeout, volatile int *const abort_wait);
#ifdef __cplusplus
}
#endif
#endif
