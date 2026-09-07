# OptaEtherNetIP – OpENer File Modifications

This document records the changes made to the original **OpENer** source files for the
**Arduino Opta / Mbed Ethernet** port.

The Arduino library is located at:

`source/src/ports/Arduino/OptaEtherNetIP/`

The original OpENer source remains in its original location in this repository.

Only files that were actually modified for the Opta port are listed below.

**Total modified OpENer files documented here: 14**

> **Note:** Line numbers refer to the current Arduino library copies and may move as the
> files are edited. The descriptions identify the code/section so the change can be
> located even if line numbers change.

---

## `cipioconnection.c`

**Location: approximately line 101**

### Modification

The local `ENIPMessage outgoing_message` variable used by `SendConnectedData()` was
changed to:

`static ENIPMessage outgoing_message`

The message is still reinitialised on each call.

### Reason

Uses static storage for the outgoing Class 1 I/O message structure instead of placing
the structure on the stack for each transmission.

### Protocol / functional impact

No intended EtherNet/IP protocol behaviour is changed. This is an embedded memory/
stack-usage change.

---

## `ciptypes.c`

**Location: approximately lines 8–10**

### Modification

Three local OpENer headers were changed from angle-bracket includes to quoted includes:

- `<ciptypes.h>` → `"ciptypes.h"`
- `<endianconv.h>` → `"endianconv.h"`
- `<trace.h>` → `"trace.h"`

### Reason

Improves local-header resolution when the OpENer sources are compiled as part of the
self-contained Arduino Opta library.

### Protocol / functional impact

No protocol or runtime behaviour is changed. This is a build/integration change.

---

## `cpf.c`

**Location: approximately lines 33–75 and 175**

### Modification

The `CipMessageRouterResponse` response buffer was changed from local stack storage to
static storage.

The original code created the response structure as a local variable. The Opta version
uses a static buffer and passes a pointer to that buffer through the response-building
functions.

### Reason

The message-router response structure is relatively large. Static storage reduces stack
usage on the Arduino Opta/Mbed environment.

### Protocol / functional impact

No EtherNet/IP protocol behaviour is intentionally changed. The response buffer is still
initialised for each use.

---

## `encap.c`

**Locations: approximately lines 128, 203, 282, 490, 525 and 725–733**

### 1. Transaction status reporting – around lines 128, 203 and 282

The Opta version adds transaction recording/reporting around TCP and UDP encapsulation
message processing.

This allows the Arduino application to report information such as:

`TCP RX SendRRData (42 bytes) -> TX 78 bytes : SUCCESS`

without the library itself printing the transaction.

### 2. Socket timer validation – around line 490

Additional checking was added after requesting a socket timer.

If a socket timer cannot be allocated, the code handles the failure instead of continuing
with an invalid timer reference.

### 3. `UnregisterSession` socket handling – around line 525

Socket closing was changed so that `encap.c` does not directly destroy/close the accepted
Mbed socket at the point where the encapsulation message is processed.

The network-handler layer is responsible for closing the socket.

This is important for Mbed socket lifetime handling and avoids the socket being invalidated
while it is still being processed.

### 4. Delayed UDP transmission – around lines 725–733

The delayed UDP message slot is invalidated before calling `sendto()`.

This prevents the delayed-message slot from being reused while the send operation is in
progress.

Additional handling records the result of the send operation.

### Reason

These changes address the differences between the original OpENer socket model and the
Arduino Opta/Mbed socket implementation, while also providing useful transaction status
to the Arduino application.

### Protocol / functional impact

The intended EtherNet/IP protocol behaviour is unchanged. The changes are primarily
socket-lifetime, robustness and diagnostic changes.

---

## `endianconv.c`

**Location: approximately line 9**

### Modification

The platform networking includes were changed from POSIX/lwIP/Windows networking headers
to:

`opener_mbed_socket_compat.h`

### Reason

The Arduino Opta port does not use the original POSIX/lwIP/Windows socket environment.
The Mbed socket compatibility layer provides the networking definitions required by
OpENer.

### Protocol / functional impact

No EtherNet/IP protocol behaviour is changed. This is a platform/build integration change.

---

## `generic_networkhandler.c`

**Locations: approximately lines 22, 72, 78, 365–395, 465, 495, 583, 605, 650, 673,
758, 890 and 1095**

This is one of the main platform-adaptation files.

### 1. Arduino/Mbed integration – around line 22

Arduino/Mbed-specific support is included for the Opta implementation.

### 2. File-descriptor type – around line 72

The original `fd_set` usage was changed to the Opta-compatible:

`opener_fd_set_t`

### 3. Time structure – around line 78

The original `struct timeval` usage was changed to:

`opener_timeval_t`

### 4. Startup/socket diagnostics – around lines 365–395

Additional diagnostics were added around socket-set initialisation and socket-count
initialisation.

These were used to diagnose networking startup on the Opta.

### 5. Socket timer error handling – around line 465

Socket-timer allocation is checked and failure is handled rather than relying on the
original assertion behaviour.

### 6. `select()` replacement – around line 495

The original POSIX `select()` mechanism was replaced with:

`opener_select()`

The Opta/Mbed implementation does not provide the same POSIX `select()` API, so the
compatibility layer provides the required behaviour.

### 7. Large network buffers moved to static storage

Several large receive/transmit buffers were changed to static storage, approximately at:

- UDP buffers – lines 583, 605, 650 and 673
- TCP receive buffer – line 758
- TCP transmit buffer – line 890
- UDP I/O buffer – line 1095

### Reason

The file was adapted from the original POSIX/lwIP networking implementation to work with
the Arduino Opta/Mbed networking API.

Static buffers also reduce stack usage, which is important for the embedded target.

### Protocol / functional impact

The purpose is to preserve OpENer's network behaviour while adapting its socket layer to
Mbed.

---

## `generic_networkhandler.h`

**Locations: approximately lines 20, 43 and 58**

### Modification

The platform-specific networking types were changed:

- `fd_set` → `opener_fd_set_t`
- `struct timeval` → `opener_timeval_t`

The STM32/lwIP-specific `lwip/errno.h` handling was also removed.

### Reason

The header must use the same networking abstraction as the Opta/Mbed implementation.

### Protocol / functional impact

No EtherNet/IP protocol change. This is a platform integration change.

---

## `networkconfig.h`

**Opta-specific network configuration interface**

### Modification

The original platform-specific network configuration interface was replaced with the
interface required by the Opta implementation.

The header provides the functions used by `networkconfig.cpp` for:

- MAC address handling
- IP/network configuration
- hostname handling
- waiting for an IP address

The header also uses `extern "C"` so the functions can be linked correctly between the
OpENer C code and the Arduino/C++ implementation.

### Reason

The original OpENer network configuration depends on the target platform. The Opta port
uses Arduino/Mbed Ethernet instead.

### Protocol / functional impact

No EtherNet/IP protocol change. It provides the platform-specific network configuration
layer.

---

## `networkhandler.h`

**Opta-specific network-handler interface**

### Modification

The header was simplified for the Arduino library and uses `extern "C"` around the
network-handler API.

The core OpENer network-handler interface is retained, while the platform-specific
documentation/dependencies from the original version are removed.

### Reason

Allows the OpENer C code to interface correctly with the C++ Mbed network-handler
implementation.

### Protocol / functional impact

No intended EtherNet/IP protocol change.

---

## `nvqos.h`

**Location: approximately line 21**

### Modification

The return type of:

`NvQosStore()`

was changed from:

`int`

to:

`EipStatus`

### Reason

Uses OpENer's status type consistently with the rest of the Opta implementation.

### Protocol / functional impact

No protocol change. This is an error/status handling change.

---

## `nvtcpip.h`

**Location: approximately line 21**

### Modification

The return type of:

`NvTcpipStore()`

was changed from:

`int`

to:

`EipStatus`

### Reason

Uses OpENer's standard status/error type consistently.

### Protocol / functional impact

No protocol change. This is an error/status handling change.

---

## `opener.h`

**Locations: approximately lines 1–12**

### Modification

The original STM32/lwIP entry-point interface was replaced with an Arduino-facing API.

The Opta version provides:

- `opener_init()`
- `opener_process()`
- `opener_last_status()`
- `opener_running()`

The original:

`opener_init(struct netif *netif)`

interface is no longer required because the Opta uses the Arduino/Mbed Ethernet
interface directly.

`extern "C"` is also used so the API can be called from the C++ Arduino sketch.

### Reason

Provides a simple interface between the OpENer stack and the Arduino `.ino` application.

### Protocol / functional impact

No EtherNet/IP protocol change. This changes how the stack is initialised and serviced by
the application.

---

## `opener_user_conf.h`

**Opta-specific configuration file**

### Modification

The original platform configuration was replaced with an Arduino Opta configuration.

The Opta version:

- adds Arduino support
- removes the original lwIP/FreeRTOS dependencies
- defines the OpENer features used by the Opta port
- defines connection/session limits
- defines the Ethernet buffer size
- defines the OpENer timer tick
- provides the assertion configuration used by the Arduino build

Important configuration areas are approximately:

- **lines 5–8** – Arduino and OpENer includes
- **lines 9–30** – feature configuration
- **lines 32–39** – connection/session limits
- **line 40** – Ethernet buffer size
- **line 42** – OpENer timer tick
- **line 44** – assertion configuration

### Reason

The original configuration targets other platforms. The Opta requires a configuration
that matches Arduino/Mbed and the resources available on the STM32H747.

### Protocol / functional impact

The configuration controls which OpENer features and connection types are compiled into
the adapter. It is therefore platform/configuration specific rather than a change to
the underlying EtherNet/IP protocol implementation.

---

## `platform_network_includes.h`

**Locations: approximately lines 1–8**

### Modification

The original platform networking headers were replaced by the Opta/Mbed compatibility
layer:

`opener_mbed_socket_compat.h`

The file also defines the `RESTRICT` macro required by the OpENer source.

### Reason

This file acts as the platform-networking bridge between the original OpENer networking
code and the Arduino Opta/Mbed socket implementation.

### Protocol / functional impact

No intended EtherNet/IP protocol change. This is a platform abstraction/build
integration change.

---

# Summary of the Modifications

The modified files fall into four main groups.

### 1. Embedded memory / stack usage

- `cipioconnection.c`
- `cpf.c`
- `generic_networkhandler.c`

Large temporary network/message structures are moved to static storage to reduce stack
usage on the Opta.

### 2. Mbed networking adaptation

- `endianconv.c`
- `generic_networkhandler.c`
- `generic_networkhandler.h`
- `platform_network_includes.h`
- `networkconfig.h`
- `networkhandler.h`

These changes replace POSIX/lwIP assumptions with the Opta/Mbed socket compatibility
layer.

### 3. Arduino Opta application interface

- `opener.h`
- `opener_user_conf.h`

These provide the Arduino-facing initialisation, processing and configuration required
to run OpENer as an Arduino library.

### 4. Runtime robustness and diagnostics

- `encap.c`
- `nvqos.h`
- `nvtcpip.h`

These changes improve socket handling, transaction reporting and status/error handling.

---

## Files deliberately not listed

### `opener_api.h`

`opener_api.h` is **not included in the modified-file list** because the comparison shows
no functional/code modification to that file. It should therefore be treated as an
**unchanged OpENer file copied into the Arduino library**.

