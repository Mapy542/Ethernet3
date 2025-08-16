# Changelog

All notable changes to the Ethernet3 library will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [1.0.0] - 2025-8-16

### Added

-   **Multi-Instance Architecture**: Support for multiple Ethernet chips simultaneously
-   **Universal Chip Support**: Full W5100 and W5500 compatibility
-   **UDP Multicast**: Comprehensive multicast support with automatic group management
-   **Link Status Monitoring**: `linkActive()` function for physical link detection
-   **Platform Optimizations**: Hardware-specific optimizations for ESP32, STM32, Arduino
-   **Multi-Instance Network Classes**: All classes support association with specific Ethernet instances
-   **PlatformIO Compatibility**: Complete PlatformIO library manifest and CI/CD
-   **Comprehensive Documentation**: Complete docs in `docs/` folder
-   **GitHub Actions**: Automated testing and release workflows

### Multi-Instance Features

-   `Ethernet3Class` constructor with chip type and CS pin specification
-   Independent socket management per instance (W5500: 8 sockets, W5100: 4 sockets)
-   Per-instance source port allocation preventing conflicts
-   `EthernetClient(&eth)`, `EthernetServer(port, &eth)`, `EthernetUDP(&eth)` constructors
-   `DhcpClass(&eth)` for per-instance DHCP management

### UDP Multicast Features

-   `beginMulticast(IPAddress, port)` for starting multicast UDP
-   `joinMulticastGroup(IPAddress)` and `leaveMulticastGroup(IPAddress)`
-   Automatic multicast MAC calculation (01:00:5e:XX:XX:XX format)
-   W5500/W5100 hardware configuration for multicast reception
-   IGMP considerations and router compatibility guidance

### Platform Support

-   **ESP32**: 16MHz SPI, FreeRTOS integration, dual-core support, critical sections
-   **STM32**: DMA transfers, hardware acceleration, 12MHz SPI, fast GPIO
-   **Arduino**: Conservative optimizations, yield support, full compatibility
-   **Automatic Detection**: Platform-specific optimizations applied automatically

### Documentation

-   Complete getting started guide
-   Multi-instance architecture explanation
-   UDP multicast tutorial with examples
-   Platform support and optimization details
-   API reference with all methods documented
-   Migration guide from Ethernet/Ethernet2
-   Troubleshooting guide with diagnostic tools
-   Examples walkthrough and usage patterns

### Backward Compatibility

-   All existing Ethernet2 code works unchanged
-   Global `Ethernet` instance maintained for compatibility
-   `EthernetClass` typedef for legacy code
-   Identical API surface for basic operations
-   Gradual migration path with opt-in new features

### Migration from Previous Versions

-   Replace `#include <Ethernet2.h>` with `#include <Ethernet3.h>`
-   All existing code continues to work unchanged
-   New features are opt-in and don't affect existing functionality
-   See [Migration Guide](docs/migration-guide.md) for details

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on how to contribute to this changelog and the project.
