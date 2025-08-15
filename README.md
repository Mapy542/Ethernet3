# Ethernet3 Library

A modernized Ethernet library with multi-instance architecture, W5100/W5500 support, UDP multicast, and comprehensive platform optimizations.

[![PlatformIO Registry](https://badges.registry.platformio.org/packages/ethernet3/library/Ethernet3.svg)](https://registry.platformio.org/libraries/ethernet3/Ethernet3)
[![Arduino Library](https://www.ardu-badge.com/badge/Ethernet3.svg)](https://www.ardu-badge.com/Ethernet3)
[![GitHub release](https://img.shields.io/github/release/Mapy542/Ethernet3.svg)](https://github.com/Mapy542/Ethernet3/releases)
[![License: LGPL v2.1](https://img.shields.io/badge/License-LGPL%20v2.1-blue.svg)](https://www.gnu.org/licenses/lgpl-2.1)

## 🚀 Key Features

### Flexible Backward Compatibility
Choose between modern multi-instance API or full Ethernet/Ethernet2 compatibility:

```cpp
// Legacy code works unchanged (default)
#include <Ethernet3.h>
Ethernet.begin(mac);
EthernetServer server(80);

// Or use modern explicit instances  
#define ETHERNET3_NO_BACKWARDS_COMPATIBILITY
#include <Ethernet3.h>
Ethernet3Class eth(CHIP_TYPE_W5500, 10);
EthernetServer server(80, &eth);
```

### Multi-Instance Architecture
Use multiple Ethernet chips simultaneously:
```cpp
Ethernet3Class eth1(CHIP_TYPE_W5500, 10);  // W5500 on CS pin 10
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);   // W5100 on CS pin 9
eth1.begin(mac1, ip1);  // 8 sockets available
eth2.begin(mac2, ip2);  // 4 sockets available
```

### Universal Chip Support
- **W5500**: 8 sockets, hardware TCP/IP, full feature set
- **W5100**: 4 sockets, restored from Ethernet2 heritage

### Advanced UDP Multicast
```cpp
EthernetUDP udp;
IPAddress multicastGroup(239, 255, 0, 1);
udp.beginMulticast(multicastGroup, 8080);
// Automatic multicast MAC calculation and hardware configuration
```

### Link Status Monitoring
```cpp
if (Ethernet.linkActive()) {
  Serial.println("Cable connected and link active");
}
```

### Platform Optimizations
- **ESP32**: 16MHz SPI, FreeRTOS integration, dual-core support
- **STM32**: DMA transfers, hardware acceleration, 12MHz SPI  
- **Arduino**: Conservative optimizations, full compatibility

### Complete Multi-Instance Network Classes
```cpp
EthernetClient client1(&eth1);     // Client uses eth1 W5500 chip
EthernetServer server1(80, &eth1);  // Server uses eth1 socket pool
EthernetUDP udp1(&eth1);           // UDP uses eth1 instance
DhcpClass dhcp1(&eth1);            // DHCP for eth1 interface
```

## 📋 Quick Start

### Installation

**PlatformIO** (Recommended):
```bash
pio lib install "Ethernet3"
```

**Arduino IDE**:
1. Download latest release from [GitHub](https://github.com/Mapy542/Ethernet3/releases)
2. Extract to `Arduino/libraries/Ethernet3/`
3. Restart Arduino IDE

### Basic Usage

```cpp
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

void setup() {
  Serial.begin(115200);
  
  // Initialize with DHCP
  if (Ethernet.begin(mac) == 0) {
    Serial.println("DHCP failed");
    // Fallback to static IP
    IPAddress ip(192, 168, 1, 177);
    Ethernet.begin(mac, ip);
  }
  
  Serial.print("IP address: ");
  Serial.println(Ethernet.localIP());
  
  if (Ethernet.linkActive()) {
    Serial.println("Ethernet cable connected");
  }
}

void loop() {
  // Your networking code here
}
```

## 📚 Documentation

- **[Getting Started](docs/getting-started.md)** - Installation and basic usage
- **[Backward Compatibility](docs/BACKWARDS_COMPATIBILITY.md)** - Legacy API support and migration
- **[Multi-Instance Architecture](docs/multi-instance.md)** - Using multiple Ethernet chips
- **[UDP Multicast](docs/udp-multicast.md)** - Group communication
- **[Platform Support](docs/platform-support.md)** - Arduino, ESP32, STM32 optimizations
- **[API Reference](docs/api-reference.md)** - Complete method documentation
- **[Examples Guide](docs/examples.md)** - Detailed example walkthroughs
- **[Migration Guide](docs/migration-guide.md)** - Upgrading from Ethernet/Ethernet2
- **[Troubleshooting](docs/troubleshooting.md)** - Common issues and solutions

## 🔧 Hardware Support

### Wiring (Arduino Uno/Nano)

| Ethernet Module | Arduino Pin |
|----------------|-------------|
| VCC | 3.3V or 5V |
| GND | GND |
| MOSI | Pin 11 |
| MISO | Pin 12 |
| SCK | Pin 13 |
| CS | Pin 10 (or custom) |

### Supported Platforms

| Platform | SPI Speed | Special Features |
|----------|-----------|------------------|
| Arduino AVR | 8MHz | Conservative, stable |
| ESP32 | 16MHz | FreeRTOS, dual-core |
| STM32F4 | 12MHz | DMA, hardware accel |
| ESP8266 | 10MHz | WiFi coexistence |
| Teensy 4.x | 30MHz | Maximum performance |

## 💡 Examples

### Basic Web Client
```cpp
#include <Ethernet3.h>

EthernetClient client;

void setup() {
  Ethernet.begin(mac);
  
  if (client.connect("httpbin.org", 80)) {
    client.println("GET /ip HTTP/1.1");
    client.println("Host: httpbin.org");
    client.println("Connection: close");
    client.println();
  }
}

void loop() {
  if (client.available()) {
    Serial.write(client.read());
  }
}
```

### Multi-Instance Setup
```cpp
#include <Ethernet3.h>

Ethernet3Class eth1(CHIP_TYPE_W5500, 10);
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);

void setup() {
  eth1.begin(mac1, ip1);  // Production network
  eth2.begin(mac2, ip2);  // Sensor network
  
  EthernetServer server1(80, &eth1);   // Web server
  EthernetUDP sensors(&eth2);          // Sensor communication
  
  server1.begin();
  sensors.begin(8888);
}
```

### UDP Multicast
```cpp
#include <Ethernet3.h>

EthernetUDP udp;
IPAddress multicastGroup(239, 255, 0, 1);

void setup() {
  Ethernet.begin(mac);
  udp.beginMulticast(multicastGroup, 8080);
}

void loop() {
  // Send multicast message
  udp.beginPacket(multicastGroup, 8080);
  udp.print("Hello multicast group!");
  udp.endPacket();
  
  // Check for incoming multicast packets
  int packetSize = udp.parsePacket();
  if (packetSize) {
    Serial.println("Received multicast packet");
  }
  
  delay(5000);
}
```

## 🔄 Backward Compatibility

All existing Ethernet2 code works unchanged:

```cpp
// This code works exactly the same
#include <Ethernet3.h>  // Just change the include

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

void setup() {
  Ethernet.begin(mac);  // Same API
}

void loop() {
  EthernetClient client;  // Same usage
  client.connect("example.com", 80);
}
```

## 🏗 Architecture

```
┌─────────────────┐    ┌─────────────────┐
│   Application   │    │   Application   │
└─────────────────┘    └─────────────────┘
         │                       │
         v                       v
┌─────────────────┐    ┌─────────────────┐
│ EthernetClient  │    │ EthernetClient  │
│ EthernetServer  │    │ EthernetServer  │
│ EthernetUDP     │    │ EthernetUDP     │
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
│Platform HAL/Bus │    │Platform HAL/Bus │
│   (ESP32/STM32/ │    │   (ESP32/STM32/ │
│    Arduino)     │    │    Arduino)     │
└─────────────────┘    └─────────────────┘
```

## 📦 PlatformIO Release

See [PLATFORMIO_RELEASE.md](PLATFORMIO_RELEASE.md) for automated release instructions.

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests and documentation
5. Submit a pull request

## 📄 License

Copyright (c) 2009-2024 Arduino LLC and Ethernet3 Contributors. All rights reserved.

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

## 🔗 Links

- [W5100 Datasheet](http://www.wiznet.co.kr/product-item/w5100/)
- [W5500 Datasheet](http://www.wiznet.co.kr/product-item/w5500/)
- [Arduino Ethernet Shield](https://www.arduino.cc/en/Main/ArduinoEthernetShield)
- [PlatformIO Registry](https://registry.platformio.org/libraries/ethernet3/Ethernet3)
