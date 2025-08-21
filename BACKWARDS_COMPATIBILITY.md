# Ethernet3 Backwards Compatibility Mode

This document describes the backwards compatibility mode for the Ethernet3 library.

## Overview

Ethernet3 uses a modern dependency injection approach where you explicitly create chip instances and pass them to Ethernet classes:

```cpp
// Modern Ethernet3 approach
W5500 chip(10);
EthernetClass ethernet(&chip);
EthernetClient client(&ethernet, &chip);
EthernetServer server(&ethernet, &chip, 80);
```

However, for backwards compatibility with older Ethernet library code, you can enable a compatibility mode that provides global singleton instances and simplified constructors.

## Enabling Backwards Compatibility

To enable backwards compatibility mode, define `ETHERNET_BACKWARDS_COMPATIBILITY` before including the Ethernet3 library:

```cpp
#define ETHERNET_BACKWARDS_COMPATIBILITY
#include <Ethernet3.h>
```

## Using Backwards Compatibility Mode

When backwards compatibility is enabled, you can use the library in the traditional way:

```cpp
#define ETHERNET_BACKWARDS_COMPATIBILITY
#include <Ethernet3.h>
#include <SPI.h>

byte mac[] = {0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02};

// Simple constructors - no chip/ethernet parameters needed
EthernetClient client;
EthernetServer server(80);
EthernetUDP udp;

void setup() {
  Serial.begin(9600);
  
  // Use the global Ethernet instance
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    for (;;);
  }
  
  server.begin();
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  EthernetClient client = server.available();
  if (client) {
    // Handle client...
    client.stop();
  }
}
```

## Global Instances

When backwards compatibility is enabled, the following global instances are available:

- `W5500 defaultChip` - Default W5500 chip instance using CS pin 10
- `EthernetClass Ethernet` - Global ethernet instance using the default chip

## Available Constructors

The following simplified constructors are available in backwards compatibility mode:

- `EthernetClient()` - Default constructor
- `EthernetClient(uint8_t sock)` - Constructor with socket number
- `EthernetServer(uint16_t port)` - Constructor with port only
- `EthernetUDP()` - Default constructor
- `DNSClient()` - Default constructor
- `DNSClient(unsigned long timeout)` - Constructor with timeout
- `DhcpClass(unsigned long timeout, unsigned long responseTimeout)` - Constructor with timeouts

## Chip Selection

The backwards compatibility mode uses the W5500 chip by default with CS pin 10. If you need to use a different chip or CS pin, you should use the modern dependency injection approach instead.

## Migration Guide

### From Old Ethernet Library to Backwards Compatibility Mode

If you have code written for an older Ethernet library, simply add the compatibility define:

```cpp
// Add this line at the top
#define ETHERNET_BACKWARDS_COMPATIBILITY

// Your existing code should work unchanged
#include <Ethernet.h>  // Change to <Ethernet3.h>
// ... rest of your code
```

### From Backwards Compatibility to Modern Mode

To migrate from backwards compatibility to the modern approach:

1. Remove the `#define ETHERNET_BACKWARDS_COMPATIBILITY` line
2. Create explicit chip and ethernet instances
3. Pass these instances to all constructors

Before:
```cpp
#define ETHERNET_BACKWARDS_COMPATIBILITY
#include <Ethernet3.h>

EthernetClient client;
```

After:
```cpp
#include <Ethernet3.h>

W5500 chip(10);
EthernetClass ethernet(&chip);
EthernetClient client(&ethernet, &chip);
```

## Examples

See the `examples/BackwardsCompatibilityTest/` directory for a complete working example using backwards compatibility mode.