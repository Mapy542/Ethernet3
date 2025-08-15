# Changelog

All notable changes to the Ethernet3 library will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.6.0] - 2024-01-15

### Added
- **Multi-Instance Architecture**: Support for multiple Ethernet chips simultaneously
- **Universal Chip Support**: Full W5100 and W5500 compatibility
- **UDP Multicast**: Comprehensive multicast support with automatic group management
- **Link Status Monitoring**: `linkActive()` function for physical link detection
- **Platform Optimizations**: Hardware-specific optimizations for ESP32, STM32, Arduino
- **Multi-Instance Network Classes**: All classes support association with specific Ethernet instances
- **PlatformIO Compatibility**: Complete PlatformIO library manifest and CI/CD
- **Comprehensive Documentation**: Complete docs in `docs/` folder
- **GitHub Actions**: Automated testing and release workflows

### Multi-Instance Features
- `Ethernet3Class` constructor with chip type and CS pin specification
- Independent socket management per instance (W5500: 8 sockets, W5100: 4 sockets)
- Per-instance source port allocation preventing conflicts
- `EthernetClient(&eth)`, `EthernetServer(port, &eth)`, `EthernetUDP(&eth)` constructors
- `DhcpClass(&eth)` for per-instance DHCP management

### UDP Multicast Features
- `beginMulticast(IPAddress, port)` for starting multicast UDP
- `joinMulticastGroup(IPAddress)` and `leaveMulticastGroup(IPAddress)`
- Automatic multicast MAC calculation (01:00:5e:XX:XX:XX format)
- W5500/W5100 hardware configuration for multicast reception
- IGMP considerations and router compatibility guidance

### Platform Support
- **ESP32**: 16MHz SPI, FreeRTOS integration, dual-core support, critical sections
- **STM32**: DMA transfers, hardware acceleration, 12MHz SPI, fast GPIO
- **Arduino**: Conservative optimizations, yield support, full compatibility
- **Automatic Detection**: Platform-specific optimizations applied automatically

### Documentation
- Complete getting started guide
- Multi-instance architecture explanation  
- UDP multicast tutorial with examples
- Platform support and optimization details
- API reference with all methods documented
- Migration guide from Ethernet/Ethernet2
- Troubleshooting guide with diagnostic tools
- Examples walkthrough and usage patterns

### Examples
- `Ethernet3ComprehensiveDemo`: Full-featured demonstration
- `MultiInstanceDemo`: Basic multi-instance setup
- `Ethernet3ModernizationDemo`: Link monitoring and multicast
- `MultiInstanceClientServerDemo`: Complete multi-instance networking
- `UDPMultiInstanceDemo`: UDP with multiple instances
- `PlatformOptimizationDemo`: Platform-specific features

### Development & Release
- PlatformIO library manifest (`library.json`)
- GitHub Actions for automated testing and releases
- Comprehensive CI/CD pipeline testing multiple platforms
- Automated PlatformIO registry publishing
- Release guide with workflow documentation

### Changed
- Modernized codebase architecture while maintaining backward compatibility
- Enhanced error handling and stability
- Improved memory management and performance
- Updated examples with modern C++ practices

### Fixed
- W5100 chip support fully restored and enhanced
- Socket allocation conflicts in multi-client scenarios
- Memory leaks in long-running applications
- SPI communication reliability across platforms
- DHCP lease renewal edge cases

### Backward Compatibility
- All existing Ethernet2 code works unchanged
- Global `Ethernet` instance maintained for compatibility
- `EthernetClass` typedef for legacy code
- Identical API surface for basic operations
- Gradual migration path with opt-in new features

## [1.5.x] - Historical Ethernet2 Versions

Previous versions were based on the Ethernet2 library. Version 1.6.0 represents a complete modernization while maintaining full backward compatibility.

### Migration from Previous Versions
- Replace `#include <Ethernet2.h>` with `#include <Ethernet3.h>`
- All existing code continues to work unchanged
- New features are opt-in and don't affect existing functionality
- See [Migration Guide](docs/migration-guide.md) for details

## Unreleased Features

### Planned for Future Versions
- **Additional Chip Support**: W5200, W6100 evaluation
- **IPv6 Support**: Modern network protocol support
- **TLS/SSL Integration**: Secure communication layer
- **Advanced QoS**: Traffic prioritization and bandwidth management
- **Mesh Networking**: Multi-hop communication protocols
- **SNMP Support**: Network management protocol
- **Web Framework**: High-level web application framework

### Platform Expansions
- **RISC-V**: Support for emerging RISC-V architectures
- **nRF5x**: Nordic Semiconductor wireless SoCs
- **RP2040**: Enhanced Raspberry Pi Pico support
- **ESP32-S3/C3**: Latest ESP32 variants optimization

---

## Version Support

| Version | Status | Support Level | End of Life |
|---------|--------|---------------|-------------|
| 1.6.x | Current | Full support | - |
| 1.5.x | Legacy | Security fixes only | 2025-01-01 |
| < 1.5 | Deprecated | No support | Ended |

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on how to contribute to this changelog and the project.