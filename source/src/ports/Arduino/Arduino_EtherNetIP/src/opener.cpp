#include <Arduino.h>
#include <stdio.h>

extern "C" {
#include "opener.h"
#include "opener_api.h"
#include "cipethernetlink.h"
#include "ciptcpipinterface.h"
#include "trace.h"
#include "networkconfig.h"
#include "generic_networkhandler.h"
}

static bool g_running = false;
static char g_last_status[192] = "No request processed";

static const char *command_name(unsigned int command) {
  switch (command) {
    case 0x0060: return "ListServices";
    case 0x0063: return "ListIdentity";
    case 0x0064: return "ListInterfaces";
    case 0x0065: return "RegisterSession";
    case 0x0066: return "UnregisterSession";
    case 0x006F: return "SendRRData";
    case 0x0070: return "SendUnitData";
    case 0x0000: return "NOP";
    default: return "Unknown";
  }
}

extern "C" void OpenerRecordTransaction(const char *transport, unsigned int command,
                                         unsigned long request_bytes, long response_bytes, bool success) {
  snprintf(g_last_status, sizeof(g_last_status),
           "%s RX %s (%lu bytes) -> TX %ld bytes : %s",
           transport ? transport : "NET", command_name(command), request_bytes,
           response_bytes, success ? "SUCCESS" : "FAILED");
}

const char *opener_last_status(void) { return g_last_status; }


void opener_init(void) {
  NetworkInterface *net = opener_get_network();
  if (!net) {
    g_running = false;
    return;
  }

  uint8_t mac[6] = {0};
  IfaceGetMacAddress(nullptr, mac);

  SetDeviceSerialNumber(123456789UL);
  SetDeviceRevision(1, 0);
  SetDeviceProductName("Arduino Opta OpENer");

  EipStatus s = CipStackInit((EipUint16)(millis() & 0xFFFF));
  if (s != kEipStatusOk) { g_running = false; return; }

  CipEthernetLinkSetMac(mac);
  GetHostName(&g_tcpip.hostname);

  if (IfaceGetConfiguration(nullptr, &g_tcpip.interface_configuration) != kEipStatusOk) {
    ShutdownCipStack();
    g_running = false;
    return;
  }
  if (NetworkHandlerInitialize() != kEipStatusOk) {
    ShutdownCipStack();
    g_running = false;
    return;
  }
  g_running = true;
}

const char *opener_process(void) {
  if (!g_running) {
    snprintf(g_last_status, sizeof(g_last_status), "OpENer not running");
    return g_last_status;
  }
  if (!opener_get_network()) {
    snprintf(g_last_status, sizeof(g_last_status), "Network unavailable : FAILED");
    g_running = false;
    NetworkHandlerFinish();
    ShutdownCipStack();
    return g_last_status;
  }
  snprintf(g_last_status, sizeof(g_last_status), "No request processed");
  EipStatus cyclic_status = NetworkHandlerProcessCyclic();
  if (cyclic_status != kEipStatusOk) {
    snprintf(g_last_status, sizeof(g_last_status), "Network processing : FAILED");
    g_running = false;
    NetworkHandlerFinish();
    ShutdownCipStack();
  }
  return g_last_status;
}

bool opener_running(void) { return g_running; }
