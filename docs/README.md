# Ethernet3 Library Documentation

Welcome to the comprehensive documentation for the Ethernet3 library - a modernized, multi-instance Ethernet library with support for W5100 and W5500 chips.

## Quick Navigation

- [Getting Started](getting-started.md) - Installation and basic usage
- [Multi-Instance Architecture](multi-instance.md) - Using multiple Ethernet chips simultaneously  
- [UDP Multicast](udp-multicast.md) - Advanced multicast networking
- [Platform Support](platform-support.md) - Arduino, ESP32, STM32 optimization
- [API Reference](api-reference.md) - Complete class and method documentation
- [Examples](examples.md) - Detailed example walkthroughs
- [Migration Guide](migration-guide.md) - Upgrading from Ethernet/Ethernet2
- [Troubleshooting](troubleshooting.md) - Common issues and solutions

## Key Features

### 🚀 Multi-Instance Architecture
Use multiple Ethernet chips simultaneously in the same application:

```cpp
// Different chips, different purposes
Ethernet3Class eth1(CHIP_TYPE_W5500, 10);  // Main networking
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);   // Sensor network

eth1.begin(mac1, ip1);  // 8 sockets available  
eth2.begin(mac2, ip2);  // 4 sockets available
```

### 🔌 Universal Chip Support
- **W5500**: 8 sockets, hardware TCP/IP, full feature set
- **W5100**: 4 sockets, restored from Ethernet2 heritage, stable operation

### 📡 Advanced UDP Multicast
Built-in multicast support with automatic group management:

```cpp
EthernetUDP udp;
IPAddress multicastGroup(239, 255, 0, 1);

udp.beginMulticast(multicastGroup, 8080);
udp.joinMulticastGroup(multicastGroup);
// Automatic multicast MAC calculation and hardware configuration
```

### 🎯 Complete Network Classes
All classes support multi-instance operation:

- **EthernetClient**: Per-instance source port management
- **EthernetServer**: Instance-specific socket pools  
- **EthernetUDP**: Chip-agnostic with per-instance DNS
- **DhcpClass**: Independent DHCP clients

### ⚡ Platform Optimizations
Hardware-specific optimizations for maximum performance:

- **ESP32**: FreeRTOS integration, 16MHz SPI, dual-core support
- **STM32**: DMA transfers, hardware acceleration, 12MHz SPI
- **Arduino**: Conservative optimizations, full compatibility

### 🔗 Link Status Monitoring
Real-time physical link detection:

```cpp
if (Ethernet.linkActive()) {
  Serial.println("Cable connected");
}
```

### 🔄 Backward Compatibility
All existing Ethernet2 code works unchanged:

```cpp
// Existing code continues to work
EthernetClient client;
client.connect("example.com", 80);
```

## Architecture Overview

```
┌─────────────────┐    ┌─────────────────┐
│   Application   │    │   Application   │
│      Code       │    │      Code       │
└─────────────────┘    └─────────────────┘
         │                       │
         v                       v
┌─────────────────┐    ┌─────────────────┐
│ EthernetClient  │    │ EthernetClient  │
│ EthernetServer  │    │ EthernetServer  │
│ EthernetUDP     │    │ EthernetUDP     │
│ DhcpClass       │    │ DhcpClass       │
└─────────────────┘    └─────────────────┘
         │                       │
         v                       v
┌─────────────────┐    ┌─────────────────┐
│ Ethernet3Class  │    │ Ethernet3Class  │
│    (W5500)      │    │    (W5100)      │
└─────────────────┘    └─────────────────┘
         │                       │
         v                       v
┌─────────────────┐    ┌─────────────────┐
│   W5500Chip     │    │   W5100Chip     │
└─────────────────┘    └─────────────────┘
         │                       │
         v                       v
┌─────────────────┐    ┌─────────────────┐
│Platform HAL/Bus │    │Platform HAL/Bus │
│   (ESP32/STM32/ │    │   (ESP32/STM32/ │
│    Arduino)     │    │    Arduino)     │
└─────────────────┘    └─────────────────┘
```

## Quick Start

### 1. Installation

**PlatformIO** (Recommended):
```bash
pio lib install "Ethernet3"
```

**Arduino IDE**:
1. Download latest release from GitHub
2. Extract to `Arduino/libraries/Ethernet3/`
3. Restart Arduino IDE

### 2. Basic Usage

```cpp
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);

void setup() {
  Ethernet.begin(mac, ip);
  
  if (Ethernet.linkActive()) {
    Serial.println("Ethernet connected");
  }
}

void loop() {
  // Your networking code here
}
```

### 3. Multi-Instance Example

```cpp
#include <Ethernet3.h>

// Create two Ethernet instances
Ethernet3Class eth1(CHIP_TYPE_W5500, 10);
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);

// Network configurations
byte mac1[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte mac2[] = {0xCA, 0xFE, 0xBA, 0xBE, 0xFA, 0xCE};
IPAddress ip1(192, 168, 1, 177);
IPAddress ip2(10, 0, 0, 177);

void setup() {
  // Initialize both interfaces
  eth1.begin(mac1, ip1);
  eth2.begin(mac2, ip2);
  
  // Create instance-specific clients
  EthernetClient client1(&eth1);
  EthernetClient client2(&eth2);
  
  // Use different networks simultaneously
  client1.connect("server1.com", 80);  // Via W5500
  client2.connect("server2.com", 80);  // Via W5100
}
```

## Next Steps

- Read the [Getting Started Guide](getting-started.md) for detailed setup instructions
- Explore [Examples](examples.md) for real-world usage patterns
- Check [API Reference](api-reference.md) for complete method documentation
- Review [Platform Support](platform-support.md) for optimization options

## Support

- **GitHub Issues**: [Report bugs and request features](https://github.com/Mapy542/Ethernet3/issues)
- **Examples**: Complete examples in the `examples/` directory
- **Community**: Share your projects and get help from other users