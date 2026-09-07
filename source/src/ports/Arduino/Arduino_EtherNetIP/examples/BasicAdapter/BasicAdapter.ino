#include <Arduino.h>
#include "EthernetInterface.h"
#include "opener.h"
#include "opener_mbed_socket_compat.h"

#ifdef connect
#undef connect
#endif

EthernetInterface net;

void setup() {
    Serial.begin(115200);
    delay(1500);

    SocketAddress ip("192.168.1.50");
    SocketAddress mask("255.255.255.0");
    SocketAddress gateway("192.168.1.1");

    Serial.println("Opta EtherNet/IP Adapter");

    nsapi_error_t err = net.set_network(ip, mask, gateway);
    Serial.print("set_network = ");
    Serial.println(err);
    if (err != NSAPI_ERROR_OK) {
        Serial.println("set_network FAILED");
        return;
    }

    err = net.connect();
    Serial.print("connect = ");
    Serial.println(err);
    if (err != NSAPI_ERROR_OK) {
        Serial.println("Ethernet connection FAILED");
        return;
    }

    SocketAddress actual;
    net.get_ip_address(&actual);
    Serial.print("IP: ");
    Serial.println(actual.get_ip_address());

    Serial.println("Starting OptaEtherNetIP Example");
    opener_set_network(&net);
    opener_init();
}

void loop() {
    const char *status = opener_process();

    // Print only when a real EtherNet/IP transaction occurred.
    // opener_process() resets the status to "No request processed" on each
    // pass, so the same transaction is not printed repeatedly while idle.
    if (strcmp(status, "No request processed") != 0) {
        Serial.print("[EtherNet/IP] ");
        Serial.println(status);
    }
}
