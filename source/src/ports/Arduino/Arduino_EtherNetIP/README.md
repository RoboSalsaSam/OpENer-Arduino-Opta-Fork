# OptaEtherNetIP

EtherNet/IP adapter support for the **Arduino Opta**, based on the [OpENer](https://github.com/EIPStackGroup/OpENer) EtherNet/IP stack and adapted for the Arduino Mbed Ethernet environment.

> **Project status:** Early development / experimental.
>
> The current implementation is intended for the Arduino Opta and has been tested with Arduino Mbed OS Opta Boards 4.6.0. The networking and EtherNet/IP explicit-message path has been tested with the Hilscher EtherNet/IP Tool (https://hilscher.atlassian.net/wiki/spaces/EIS2V7/pages/123994946/Hilscher+EtherNet+IP+Tool).

## What is this?

The OptaEtherNetIP is based on OpENer and was adapted mainly via AI and not made by an expert

The Arduino-specific implementation is kept under:

```text
source/src/ports/Arduino/OptaEtherNetIP/
```

The original OpENer source remains in its normal upstream locations elsewhere in the repository.

The Arduino port contains the files required to build the Opta implementation as a self-contained Arduino library. Some of these files are unchanged copies of files from OpENer; others are modified OpENer files or new Arduino/Mbed-specific files.

## Current capabilities

The current tested implementation supports the basic EtherNet/IP communication required for explicit messaging, including:

- Ethernet connection using the Opta's built-in Ethernet interface
- Static IP configuration
- EtherNet/IP TCP communication on port `44818`
- EtherNet/IP UDP communication used by the stack
- List Identity
- Register Session
- Unregister Session
- SendRRData
- CIP Message Router
- Explicit CIP requests
- Identity Object (`0x01`)
- TCP/IP Interface Object (`0xF5`)
- Ethernet Link Object (`0xF6`)
- Basic connection/session handling

### Not yet completed

The following are planned or require additional configuration and testing:

- Arduino-friendly Identity configuration API
- Input and Output Assembly configuration
- Class 1 cyclic I/O
- Forward Open / Forward Close configuration for application I/O
- RPI configuration
- Mapping EtherNet/IP process data to Arduino variables
- Higher-level Arduino C++ API
- Additional connection and diagnostic APIs

## Arduino library structure

```text
OptaEtherNetIP/
├── library.properties
├── README.md
├── docs/
│   └── STRUCTURE.md
├── examples/
│   └── BasicAdapter/
│       └── BasicAdapter.ino
└── src/
    ├── OpENer source files
    ├── modified OpENer files
    └── Arduino / Opta / Mbed files
```

The library is deliberately kept self-contained so that the Arduino library can eventually be installed without requiring the user to understand the rest of the OpENer repository.

## Relationship to OpENer

This project is based on the **EIPStackGroup/OpENer** EtherNet/IP stack.

The upstream OpENer source tree is retained in this repository. The Arduino Opta implementation is maintained separately under:

```text
source/src/ports/Arduino/OptaEtherNetIP/
```

### File classifications

Files in the Arduino port fall into three groups:

1. **Unchanged OpENer files**  
   Copies of upstream OpENer files included so the Arduino library is self-contained.

2. **Modified OpENer files**  
   Files originally from OpENer that have been changed to support the Arduino Opta / Mbed implementation.

3. **Arduino/Opta-specific files**  
   Files created specifically for the Arduino port, including the Mbed socket compatibility layer and Arduino integration.

## Basic example

The `BasicAdapter` example starts the Opta Ethernet interface and runs the OpENer stack.

A simplified application loop is:

```cpp
void loop() {
    const char *status = opener_process();

    if (strcmp(status, "No request processed") != 0) {
        Serial.print("[EtherNet/IP] ");
        Serial.println(status);
    }
}
```

`opener_process()` does not print to `Serial` itself. It returns a short status string describing the most recent EtherNet/IP transaction. This allows an application to decide whether, when, and where diagnostic information should be displayed.

When there is no EtherNet/IP traffic, the example produces no transaction output.

## Hardware

Current target:

- Arduino Opta
- Built-in Ethernet interface

The implementation currently depends on the Arduino Mbed networking APIs used by the Opta board package.

## Software

The current development setup has been tested with:

- Arduino IDE
- Arduino Mbed OS Opta Boards **4.6.0**
- Arduino Opta
- Hilscher EtherNet/IP Tool

## Network example

A typical test setup is:

```text
PC Ethernet adapter
192.168.1.45 / 255.255.255.0
          │
          │ Ethernet
          │
Arduino Opta
192.168.1.50 / 255.255.255.0
```

EtherNet/IP TCP uses port:

```text
44818
```

## Testing

The current development testing has concentrated on explicit EtherNet/IP communication with the Hilscher EtherNet/IP Tool.

Tested operations include:

- List Identity
- Register Session
- Explicit CIP requests
- Identity attribute requests
- SendRRData
- Unregister Session
- Repeated connections and requests

The socket lifetime handling for accepted Mbed TCP sockets is an important part of the current Opta port and should not be changed without regression testing.

## Development approach

The Arduino port is being developed incrementally. The priority is to keep the known-working Ethernet/OpENer implementation stable while adding higher-level Arduino functionality.

Future changes should:

- Keep the upstream OpENer source identifiable.
- Clearly distinguish unchanged, modified, and new files.
- Avoid unnecessary changes to the working Mbed socket layer.
- Include a version update in the Arduino example when a tested revision changes.
- Test existing explicit messaging before adding new EtherNet/IP functionality.

## Licence and attribution

This project is based on OpENer by the **EIPStackGroup**. The original OpENer licence and copyright information are retained in this repository.

Please refer to `license.txt` in the repository for the applicable licence terms.

Upstream project:

https://github.com/EIPStackGroup/OpENer

## Disclaimer

This is a development project and should not currently be treated as a production-certified EtherNet/IP implementation. Thorough testing is required before using it in or production machinery and machinery safety must be derived from elsewhere.
