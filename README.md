# Ethernet3 Library Documentation

[![PlatformIO Registry](https://badges.registry.platformio.org/packages/mapy5542/library/Ethernet3.svg)](https://registry.platformio.org/libraries/mapy5542/Ethernet3)
[![GitHub release](https://img.shields.io/github/release/Mapy542/Ethernet3.svg)](https://github.com/Mapy542/Ethernet3/releases)
[![License: LGPL v2.1](https://img.shields.io/badge/License-LGPL%20v2.1-blue.svg)](https://www.gnu.org/licenses/lgpl-2.1)

## Warning

This library is in-progress and may contain bugs or incomplete features. Please use with caution and report any issues you encounter.

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

-   **TCP Client**: Full-featured TCP client with DNS resolution
-   **TCP Server**: Multi-client TCP server with broadcasting capabilities
-   **UDP Communication**: Complete UDP implementation with multicast support
-   **HTTP Client**: High-level HTTP client with support for GET, POST, PUT, DELETE methods
-   **HTTP Server**: HTTP server with routing system for web interfaces and REST APIs
-   **DHCP Client**: Automatic network configuration with lease management
-   **DNS Client**: Hostname resolution for both TCP and UDP connections

### Hardware Abstraction

-   **Multiple Chip Support**: W5100, W5500, and future WIZnet chips

## Supported Hardware

### WIZnet Chips

-   **W5500**: Primary target with full feature support
-   **W5100**: Full support (WIP)
-   **Future chips**: Extensible architecture for new WIZnet chips

### Arduino Boards

-   Arduino Uno/Nano/Pro Mini
-   Arduino Mega/Mega 2560
-   Arduino Leonardo/Micro
-   ESP32 (with appropriate SPI configuration) (WIP, server base class differences)
-   Other Arduino-compatible boards with SPI support

### Ethernet Shields/Modules

-   Arduino Ethernet Shield 2 (W5500-based)
-   WIZ550io modules
-   Custom W5500/W5100 boards
-   Any WIZnet-based Ethernet module with SPI interface

## Getting Started

### Installation

1. Download the Ethernet3 library
2. Extract to your Arduino libraries folder
3. Restart the Arduino IDE
4. Include the library in your sketch:

```cpp
#include <Ethernet3.h>
```

See examples for usage patterns.

## Next Steps

-   Review the [API Reference](docs/api-reference.md) for detailed function documentation
-   See the [HTTP Implementation Guide](docs/http-guide.md) for HTTP client and server usage
-   See the [Migration Guide](docs/migration-guide.md) for porting existing code
-   Browse [Examples](examples/) for practical usage patterns
