# Ethernet3 Library Documentation

## Overview

The Ethernet3 library is an enhanced Arduino Ethernet library designed to provide improved functionality and performance for WIZnet-based Ethernet shields and modules. It offers significant improvements over the standard Arduino Ethernet library while maintaining compatibility with existing Arduino networking APIs.

## Table of Contents

1. [Library Features](#library-features)
2. [Key Differences from Standard Arduino Ethernet](#key-differences-from-standard-arduino-ethernet)
3. [Supported Hardware](#supported-hardware)
4. [Getting Started](#getting-started)
5. [API Reference](#api-reference)
6. [Advanced Features](#advanced-features)
7. [Migration Guide](#migration-guide)
8. [Examples](#examples)
9. [Troubleshooting](#troubleshooting)

## Library Features

### Core Networking
- **TCP Client**: Full-featured TCP client with DNS resolution
- **TCP Server**: Multi-client TCP server with broadcasting capabilities
- **UDP Communication**: Complete UDP implementation with multicast support
- **DHCP Client**: Automatic network configuration with lease management
- **DNS Client**: Hostname resolution for both TCP and UDP connections

### Enhanced Functionality
- **Abstract Chip Interface**: Support for multiple WIZnet chip types (W5100, W5500, etc.)
- **Multicast UDP**: Join/leave multicast groups with automatic MAC address calculation
- **Improved Error Handling**: Better connection state management and error reporting
- **Enhanced DHCP**: Automatic lease renewal and rebinding with detailed status reporting
- **Socket Management**: Intelligent socket allocation and reuse

### Hardware Abstraction
- **Multiple Chip Support**: W5100, W5500, and future WIZnet chips
- **Flexible CS Pin**: Configurable chip select pin for hardware flexibility
- **SPI Optimization**: Optimized SPI communication for better performance

## Key Differences from Standard Arduino Ethernet

### Architecture Improvements

#### 1. Abstract Chip Interface
**Standard Arduino Ethernet**:
- Hard-coded for W5100 chip
- Direct register access throughout code
- Difficult to add support for new chips

**Ethernet3 Library**:
- Abstract `EthernetChip` interface
- Pluggable chip implementations (W5500, W5100, etc.)
- Easy to extend for future chip types

```cpp
// Standard Arduino Ethernet (singleton, W5100 only)
Ethernet.begin(mac);

// Ethernet3 (flexible, multiple chip support)
W5500 chip(10);  // CS pin 10
EthernetClass ethernet(&chip);
ethernet.begin(mac);
```

#### 2. Non-Singleton Design
**Standard Arduino Ethernet**:
- Global singleton `Ethernet` object
- Single instance per application
- Difficult to manage multiple interfaces

**Ethernet3 Library**:
- Multiple instances supported
- Better resource management
- More flexible application architectures

#### 3. Enhanced Socket Management
**Standard Arduino Ethernet**:
- Basic socket allocation
- Limited socket reuse
- Manual socket state management

**Ethernet3 Library**:
- Intelligent socket allocation
- Automatic socket cleanup
- Better connection state tracking

### Functional Enhancements

#### 1. Multicast UDP Support
**Standard Arduino Ethernet**:
- Basic UDP unicast only
- No multicast group management
- Limited broadcast capabilities

**Ethernet3 Library**:
- Full multicast group support
- Automatic multicast MAC calculation
- Join/leave multicast groups dynamically

```cpp
// Ethernet3 multicast example
EthernetUDP udp(&ethernet, &chip);
udp.beginMulticast(IPAddress(224, 1, 1, 1), 1234);
udp.joinMulticastGroup(IPAddress(224, 2, 2, 2));
```

#### 2. Improved DHCP Management
**Standard Arduino Ethernet**:
- Basic DHCP lease handling
- Limited renewal support
- Minimal status reporting

**Ethernet3 Library**:
- Comprehensive lease management
- Automatic renewal and rebinding
- Detailed status codes and error reporting

```cpp
// Enhanced DHCP status monitoring
int status = ethernet.maintain();
switch(status) {
    case DHCP_CHECK_RENEW_OK:
        Serial.println("DHCP lease renewed successfully");
        break;
    case DHCP_CHECK_REBIND_OK:
        Serial.println("DHCP lease rebound successfully");
        break;
    // ... more detailed status handling
}
```

#### 3. Better Error Handling
**Standard Arduino Ethernet**:
- Limited error reporting
- Basic connection status
- Minimal debugging information

**Ethernet3 Library**:
- Comprehensive error codes
- Detailed connection states
- Enhanced debugging capabilities

### Performance Improvements

#### 1. Optimized SPI Communication
- More efficient register access patterns
- Reduced SPI transaction overhead
- Better handling of large data transfers

#### 2. Enhanced Buffer Management
- Improved TX/RX buffer utilization
- Better memory management
- Reduced packet loss in high-traffic scenarios

#### 3. Connection State Optimization
- Faster connection establishment
- Better keep-alive handling
- Improved connection recovery

## Supported Hardware

### WIZnet Chips
- **W5500**: Primary target with full feature support
- **W5100**: Compatible with core functionality
- **Future chips**: Extensible architecture for new WIZnet chips

### Arduino Boards
- Arduino Uno/Nano/Pro Mini
- Arduino Mega/Mega 2560
- Arduino Leonardo/Micro
- ESP32 (with appropriate SPI configuration)
- Other Arduino-compatible boards with SPI support

### Ethernet Shields/Modules
- Arduino Ethernet Shield 2 (W5500-based)
- WIZ550io modules
- Custom W5500/W5100 boards
- Any WIZnet-based Ethernet module with SPI interface

## Getting Started

### Installation

1. Download the Ethernet3 library
2. Extract to your Arduino libraries folder
3. Restart the Arduino IDE
4. Include the library in your sketch:

```cpp
#include <Ethernet3.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp2.h>
```

### Basic Setup

```cpp
#include <Ethernet3.h>
#include <chips/w5500.h>

// Create chip instance (CS pin 10)
W5500 chip(10);

// Create Ethernet instance
EthernetClass ethernet(&chip);

// MAC address for your device
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

void setup() {
    Serial.begin(9600);
    
    // Initialize Ethernet with DHCP
    if (ethernet.begin(mac) == 0) {
        Serial.println("Failed to configure Ethernet using DHCP");
        // Try static IP configuration
        IPAddress ip(192, 168, 1, 177);
        ethernet.begin(mac, ip);
    }
    
    Serial.print("My IP address: ");
    Serial.println(ethernet.localIP());
}

void loop() {
    // Maintain DHCP lease
    ethernet.maintain();
}
```

## Next Steps

- Review the [API Reference](api-reference.md) for detailed function documentation
- Check out [Advanced Features](advanced-features.md) for multicast and enhanced functionality
- See the [Migration Guide](migration-guide.md) for porting existing code
- Browse [Examples](examples/) for practical usage patterns