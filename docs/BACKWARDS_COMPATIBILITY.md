# Backward Compatibility Guide

Ethernet3 provides optional backward compatibility with legacy Ethernet and Ethernet2 code. This document explains how to use the backward compatibility features and how to disable them for a modern, clean API.

## Overview

Ethernet3 is designed as a modern, multi-instance networking library that can optionally provide backward compatibility with existing code. The backward compatibility is controlled by a preprocessor directive.

## Backward Compatibility Modes

### Default Mode (Backward Compatible)

By default, Ethernet3 maintains full backward compatibility with Ethernet and Ethernet2 libraries:

```cpp
// Legacy code works unchanged
#include <Ethernet3.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

void setup() {
  Ethernet.begin(mac);  // Uses global singleton
  
  EthernetServer server(80);     // Uses global Ethernet
  EthernetClient client;         // Uses global Ethernet  
  EthernetUDP udp;              // Uses global Ethernet
}
```

**What's Available:**
- Global `Ethernet` instance (Ethernet3Class using W5500)
- `EthernetClass` typedef for compatibility
- Default constructors for all network classes
- Automatic fallback to global instance

### Modern Mode (No Backward Compatibility)

Define `ETHERNET3_NO_BACKWARDS_COMPATIBILITY` to disable backward compatibility and use the modern API:

```cpp
#define ETHERNET3_NO_BACKWARDS_COMPATIBILITY
#include <Ethernet3.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

void setup() {
  // Must explicitly create Ethernet instance
  Ethernet3Class eth(CHIP_TYPE_W5500, 10);
  eth.begin(mac);
  
  // All network classes require Ethernet instance
  EthernetServer server(80, &eth);
  EthernetClient client(&eth);
  EthernetUDP udp(&eth);
}
```

**What's Disabled:**
- No global `Ethernet` instance
- No `EthernetClass` typedef
- No default constructors for network classes
- All network classes require an Ethernet3Class instance

## Migration Strategy

### For New Projects

Use modern mode for clean, explicit code:

```cpp
#define ETHERNET3_NO_BACKWARDS_COMPATIBILITY
#include <Ethernet3.h>

// Explicit multi-instance usage
Ethernet3Class eth1(CHIP_TYPE_W5500, 10);  // Primary network
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);   // Secondary network

EthernetServer server1(80, &eth1);
EthernetServer server2(8080, &eth2);
```

### For Existing Projects

Keep backward compatibility enabled (default) and gradually migrate:

```cpp
#include <Ethernet3.h>

// Existing code works unchanged
EthernetServer server(80);  // Uses global Ethernet

// Add new modern code alongside
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);
EthernetServer server2(8080, &eth2);
```

## Platform Configuration

Configure backward compatibility in your build system:

### PlatformIO

```ini
; platformio.ini

; For backward compatibility (default)
[env:compatible]
platform = atmega328p
build_flags = 

; For modern mode
[env:modern]
platform = atmega328p
build_flags = -DETHERNET3_NO_BACKWARDS_COMPATIBILITY
```

### Arduino IDE

Add to your sketch or create a local configuration:

```cpp
// At the top of your sketch
#define ETHERNET3_NO_BACKWARDS_COMPATIBILITY
#include <Ethernet3.h>
```

## API Differences

### Constructors

| Class | Backward Compatible | Modern Only |
|-------|-------------------|-------------|
| `Ethernet3Class` | `Ethernet3Class()` | `Ethernet3Class(chip, cs_pin)` |
| `EthernetClient` | `EthernetClient()` | `EthernetClient(&eth)` |
| `EthernetServer` | `EthernetServer(port)` | `EthernetServer(port, &eth)` |
| `EthernetUDP` | `EthernetUDP()` | `EthernetUDP(&eth)` |
| `DhcpClass` | `DhcpClass()` | `DhcpClass(&eth)` |

### Global Instances

| Instance | Backward Compatible | Modern Only |
|----------|-------------------|-------------|
| `Ethernet` | Available (W5500) | Not available |
| `EthernetClass` | Typedef available | Not available |

## Benefits of Each Mode

### Backward Compatible Mode

**Pros:**
- Zero migration effort for existing code
- Can mix legacy and modern code
- Familiar API for Arduino users

**Cons:**
- Global instances always created (memory usage)
- Less explicit about which Ethernet instance is used
- Can hide multi-instance bugs

### Modern Mode

**Pros:**
- No global instances (lower memory usage)
- Explicit instance management
- Better multi-instance debugging
- Cleaner, more maintainable code

**Cons:**
- Requires updating existing code
- Slightly more verbose
- Need to explicitly manage instances

## Best Practices

### When to Use Backward Compatible Mode

- Migrating existing Arduino projects
- Prototyping with familiar API
- Teaching/learning environments
- Single Ethernet chip applications

### When to Use Modern Mode

- New projects from scratch
- Multi-instance requirements
- Memory-constrained environments
- Production/commercial applications
- Library development

## Error Handling

### Modern Mode Errors

In modern mode, if you forget to provide an Ethernet instance:

```cpp
#define ETHERNET3_NO_BACKWARDS_COMPATIBILITY
#include <Ethernet3.h>

void setup() {
  // ERROR: No backward compatibility, constructor not available
  EthernetClient client;  // Compile error
  
  // CORRECT: Must provide instance
  Ethernet3Class eth(CHIP_TYPE_W5500, 10);
  EthernetClient client(&eth);  // Works
}
```

### Runtime Behavior

When backward compatibility is disabled but null instances are used internally, methods will return error codes or fail gracefully rather than crashing.

## Future Considerations

The backward compatibility mode is intended to ease migration and may be deprecated in future major versions. New code should prefer the modern API for better maintainability and performance.