#include <Arduino.h>
#include <string.h>
#include <stdio.h>
#include "netsocket/NetworkInterface.h"
#include "netsocket/SocketAddress.h"

extern "C" {
#include "networkconfig.h"
#include "cipstring.h"
}
#include "opener_mbed_socket_compat.h"

static uint32_t socket_address_to_network(const SocketAddress &a) {
  const char *s = a.get_ip_address();
  unsigned p0, p1, p2, p3;
  if (!s || sscanf(s, "%u.%u.%u.%u", &p0, &p1, &p2, &p3) != 4) return 0;
  uint32_t host = (uint32_t(p0) << 24) | (uint32_t(p1) << 16) |
                  (uint32_t(p2) << 8) | uint32_t(p3);
  return htonl(host);
}

EipStatus IfaceGetMacAddress(TcpIpInterface *, uint8_t *const physical_address) {
  NetworkInterface *net = opener_get_network();
  if (!net || !physical_address) return kEipStatusError;
  const char *mac = net->get_mac_address();
  if (!mac) return kEipStatusError;
  unsigned int b[6];
  if (sscanf(mac, "%02x:%02x:%02x:%02x:%02x:%02x",
             &b[0], &b[1], &b[2], &b[3], &b[4], &b[5]) != 6) {
    return kEipStatusError;
  }
  for (int i = 0; i < 6; ++i) physical_address[i] = (uint8_t)b[i];
  return kEipStatusOk;
}

EipStatus IfaceGetConfiguration(TcpIpInterface *, CipTcpIpInterfaceConfiguration *cfg) {
  if (!cfg) return kEipStatusError;
  NetworkInterface *net = opener_get_network();
  if (!net) return kEipStatusError;

  SocketAddress ip, mask, gateway;
  if (net->get_ip_address(&ip) != NSAPI_ERROR_OK ||
      net->get_netmask(&mask) != NSAPI_ERROR_OK ||
      net->get_gateway(&gateway) != NSAPI_ERROR_OK) {
    return kEipStatusError;
  }

  memset(cfg, 0, sizeof(*cfg));
  cfg->ip_address = socket_address_to_network(ip);
  cfg->network_mask = socket_address_to_network(mask);
  cfg->gateway = socket_address_to_network(gateway);
  return kEipStatusOk;
}

void GetHostName(CipString *hostname) {
  SetCipStringByCstr(hostname, "Arduino-Opta");
}

EipStatus IfaceWaitForIp(TcpIpInterface *const, int timeout, volatile int *const abort_wait) {
  const unsigned long start = millis();
  while (true) {
    NetworkInterface *net = opener_get_network();
    SocketAddress ip;
    if (net && net->get_ip_address(&ip) == NSAPI_ERROR_OK && ip.get_ip_address() &&
        ip.get_ip_address()[0] != '\0') return kEipStatusOk;
    if (abort_wait && *abort_wait) return kEipStatusError;
    if (timeout >= 0 && millis() - start >= (unsigned long)timeout * 1000UL) return kEipStatusError;
    delay(10);
  }
}
