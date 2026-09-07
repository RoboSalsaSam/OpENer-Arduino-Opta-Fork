# Arduino Opta OpENer Port — File Classification

This document records the files included in the Arduino Opta EtherNet/IP
library and their relationship to the original OpENer source.

## Unchanged OpENer Files

These are unchanged copies of files from the upstream OpENer source tree.

- appcontype.c
- appcontype.h
- cipassembly.c
- cipassembly.h
- cipclass3connection.c
- cipclass3connection.h
- cipcommon.c
- cipcommon.h
- cipconnectionmanager.c
- cipconnectionmanager.h
- cipconnectionobject.c
- cipconnectionobject.h
- cipdlr.c
- cipdlr.h
- cipelectronickey.c
- cipelectronickey.h
- cipepath.c
- cipepath.h
- ciperror.h
- cipethernetlink.c
- cipethernetlink.h
- cipidentity.c
- cipidentity.h
- cipioconnection.h
- cipmessagerouter.c
- cipmessagerouter.h
- cipqos.c
- cipqos.h
- cipstring.c
- cipstring.h
- cipstringi.c
- cipstringi.h
- ciptcpipinterface.c
- ciptcpipinterface.h
- ciptypes.h
- conffile.c
- conffile.h
- cpf.h
- doublylinkedlist.c
- doublylinkedlist.h
- encap.h
- endianconv.h
- enipmessage.c
- enipmessage.h
- nvdata.c
- nvdata.h
- nvqos.c
- nvtcpip.c
- opener_api.h
- opener_error.h
- random.c
- random.h
- sampleapplication.c
- socket_timer.c
- socket_timer.h
- trace.h
- typedefs.h
- xorshiftrandom.c
- xorshiftrandom.h

**Count: 59**

## Modified OpENer Files

These files originated in OpENer and have been modified for the Arduino
Any files in STM32 port where used as the original if applicable
Opta / Mbed Ethernet implementation. Comments denote there modifications

- cipioconnection.c
- ciptypes.c
- cpf.c
- encap.c
- endianconv.c
- generic_networkhandler.c
- generic_networkhandler.h
- networkconfig.h
- networkhandler.h
- nvqos.h
- nvtcpip.h
- opener.h
- opener_user_conf.h
- platform_network_includes.h

**Count: 14**

## New Arduino / Opta Files

These files were created specifically for the Arduino Opta / Mbed
implementation and do not have a corresponding original OpENer file.

- devicedata.h
- networkconfig.cpp
- networkhandler.cpp
- opener.cpp
- opener_error.cpp
- opener_mbed_socket_compat.cpp
- opener_mbed_socket_compat.h

**Count: 7**

## Summary

| Classification | Files |
|---|---:|
| Unchanged OpENer | 58 |
| Modified OpENer | 15 |
| New Arduino / Opta | 7 |
| **Total** | **81** |

## Notes

- Unchanged files are included in the Arduino library so that the library
  can remain self-contained.
- Modified files retain the OpENer source as their starting point but
  contain changes required by the Arduino Opta / Mbed implementation.
- New files are specific to the Arduino Opta port.
- The original upstream OpENer files elsewhere in the repository are
  retained separately.
