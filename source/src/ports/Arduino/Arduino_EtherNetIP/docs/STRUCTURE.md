# OptaEtherNetIP library structure

This first library packaging revision only reorganises the existing working v42 project. No networking or OpENer functionality has been intentionally changed.

```text
OptaEtherNetIP/
├── library.properties
├── src/
│   ├── OpENer / EtherNet-IP implementation files
│   ├── Mbed/Opta network compatibility files
│   └── platform/configuration files
├── examples/
│   └── BasicAdapter/
│       └── BasicAdapter.ino
└── docs/
    └── STRUCTURE.md
```

The implementation files remain together in `src/` for this first step so existing `#include` relationships are preserved. We can split the implementation into logical subdirectories later once the library build has been verified.
