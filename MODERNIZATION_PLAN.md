# Ethernet3 Library Modernization Plan

## Overview
This document outlines the plan to modernize the Ethernet3 library (built on Ethernet2 heritage) by adding missing features from Ethernet1, supporting multiple chip instances, and making it platform-agnostic while maintaining compatibility with existing Ethernet2 functionality.

**Note**: This modernization builds upon the Ethernet2 library foundation, which originally supported both W5100 and W5500 chips. We will restore and enhance the W5100 support while ensuring full compatibility between chip types.

## Current State Analysis

### Existing Features
- **W5500 Support**: Full support for W5500 Ethernet chip via utility/w5500.h/.cpp
- **Singleton Pattern**: Single global `Ethernet` instance limiting to one chip per application
- **DHCP Support**: Built-in DHCP client functionality 
- **Timeout Functions**: Proper timeout implementation (advantage over Ethernet1)
- **Socket Management**: Support for up to 8 sockets (MAX_SOCK_NUM)
- **PHY Configuration**: Basic PHY register access via PHYCFGR

### Missing Features (from Ethernet1)
- **Link Status Checking**: No `bool linkActive()` function to check physical link status
- **Multiple Instance Support**: Cannot create multiple Ethernet objects for different chips
- **W5100 Support**: Need to add W5100 compatibility (Ethernet2 had this but it's missing)
- **Platform Abstraction**: Hard-coded Arduino SPI dependencies
- **UDP Multicast Support**: Incomplete multicast implementation lacking automatic group management

### Current Dependencies
- Arduino SPI library (hard dependency)
- Arduino-specific delay() and millis() functions
- Fixed CS pin configuration via global singleton

## Modernization Goals

### 1. Add Missing Ethernet1 Features
- **Link Status Detection**: Implement `bool linkActive()` using PHY configuration registers
- **Enhanced Connection Monitoring**: Better link state management
- **Improved Diagnostics**: Additional status and diagnostic functions

### 2. Multi-Instance Architecture
- **Remove Singleton Pattern**: Allow multiple Ethernet instances (EthernetClass, W5500Class, W5100Class, EthernetClient, EthernetServer, DhcpClass)
- **Bus Abstraction**: Create shared SPI bus management
- **Chip-Specific Classes**: Separate W5500 and W5100 implementations
- **Resource Management**: Proper CS pin and socket allocation per instance

### 3. Platform Abstraction
- **SPI Bus Interface**: Abstract SPI operations for different platforms
- **Platform HAL**: Hardware abstraction layer for timing and GPIO
- **PlatformIO Support**: Add platformio.ini configuration
- **Multi-Platform Testing**: Support Arduino, ESP32, STM32, etc.

### 4. W5100 Support
- **W5100 Driver**: Restore and enhance W5100 chip support from Ethernet2
- **Unified Interface**: Common API for both chip types
- **Feature Parity**: Ensure both chips support same functionality where possible

### 5. UDP Multicast Support
- **Complete Multicast Implementation**: Build on existing MULTI flag support
- **Automatic Group Management**: Handle IGMP group joining/leaving
- **Multicast MAC Calculation**: Automatic conversion from IP to MAC (01:00:5e:XX:XX:XX)
- **W5500 Multicast Configuration**: Proper IMR and Socket Mode Register setup
- **Multicast API**: High-level interface for joining/leaving groups and sending/receiving

## Implementation Plan

### Phase 1: Core Infrastructure Refactoring
1. **Abstract Bus Interface**
   ```cpp
   class EthernetBus {
   public:
     virtual void begin(uint8_t cs_pin) = 0;
     virtual uint8_t transfer(uint8_t data) = 0;
     virtual void transfer(uint8_t* buffer, size_t length) = 0;
     virtual void setBitOrder(uint8_t order) = 0;
     virtual void setDataMode(uint8_t mode) = 0;
     virtual void setClockDivider(uint8_t rate) = 0;
   };
   ```

2. **Platform HAL**
   ```cpp
   class EthernetHAL {
   public:
     virtual void delay(uint32_t ms) = 0;
     virtual uint32_t millis() = 0;
     virtual void digitalWrite(uint8_t pin, uint8_t value) = 0;
     virtual void pinMode(uint8_t pin, uint8_t mode) = 0;
   };
   ```

3. **Chip Abstraction**
   ```cpp
   class EthernetChip {
   protected:
     EthernetBus* bus;
     EthernetHAL* hal;
     uint8_t cs_pin;
   public:
     virtual bool init() = 0;
     virtual bool linkActive() = 0;
     virtual uint8_t getChipType() = 0;
   };
   ```

### Phase 2: Chip Implementation
1. **W5500 Refactoring**
   - Move W5500Class to inherit from EthernetChip
   - Remove global instance dependencies
   - Add link status detection via PHYCFGR register

2. **W5100 Implementation**
   - Restore W5100Class from Ethernet2 heritage, inheriting from EthernetChip
   - Implement W5100-specific register operations
   - Ensure API compatibility with W5500

3. **UDP Multicast Support**
   ```cpp
   class EthernetUDP {
   public:
     // Existing UDP functionality...
     
     // New multicast methods
     int beginMulticast(IPAddress multicast_ip, uint16_t port);
     int joinMulticastGroup(IPAddress group_ip);
     int leaveMulticastGroup(IPAddress group_ip);
     bool isMulticastGroup(IPAddress ip);
     
   private:
     void calculateMulticastMAC(IPAddress ip, uint8_t* mac);
     void configureMulticastSocket(IPAddress group_ip, uint16_t port);
   };
   ```

4. **Link Status Implementation**
   ```cpp
   bool W5500Chip::linkActive() {
     uint8_t phy_cfg = getPHYCFGR();
     return (phy_cfg & 0x01) != 0; // Check LNK bit
   }
   ```

### Phase 3: Multi-Instance Support
1. **EthernetClass Refactoring**
   - Remove singleton pattern
   - Add constructor taking chip instance and configuration
   - Implement proper resource management

2. **Singleton Classes Refactoring**
   - **W5500Class**: Remove global instance, make instantiable
   - **W5100Class**: Design as non-singleton from start
   - **EthernetClient**: Remove static _srcport, make instance-based
   - **EthernetServer**: Remove global state dependencies
   - **DhcpClass**: Make per-instance with separate UDP sockets

3. **Example Usage**
   ```cpp
   // Create bus and HAL instances
   ArduinoSPIBus bus;
   ArduinoHAL hal;
   
   // Create chip instances
   W5500Chip chip1(&bus, &hal, 10); // CS pin 10
   W5100Chip chip2(&bus, &hal, 9);  // CS pin 9
   
   // Create Ethernet instances
   EthernetClass eth1(&chip1);
   EthernetClass eth2(&chip2);
   
   // Initialize with separate DHCP instances
   eth1.begin(mac1);  // Uses internal DhcpClass instance
   eth2.begin(mac2);  // Uses separate DhcpClass instance
   
   // Check link status
   if (eth1.linkActive()) {
     // Create client instances tied to specific Ethernet
     EthernetClient client1(&eth1);
     client1.connect(server_ip, 80);
   }
   
   // Multicast support per instance
   EthernetUDP udp1(&eth1);
   udp1.beginMulticast(IPAddress(239,255,0,1), 8080);
   ```

### Phase 4: Platform Support
1. **PlatformIO Configuration**
   - Create platformio.ini with multiple environments
   - Support Arduino, ESP32, STM32, PIO frameworks

2. **Platform-Specific Implementations**
   - ArduinoSPIBus, ArduinoHAL for Arduino
   - ESP32SPIBus, ESP32HAL for ESP32
   - etc.

3. **Build System**
   - Conditional compilation for different platforms
   - Feature detection and capability flags

### Phase 5: Enhanced Features
1. **Advanced Link Monitoring**
   - Link state change callbacks
   - Connection quality metrics
   - Auto-reconnection logic

2. **Performance Optimizations**
   - Buffer management improvements
   - DMA support where available
   - Async operation modes

## UDP Multicast Implementation Details

### W5500 Multicast Requirements

The W5500 chip supports multicast reception and transmission with proper configuration:

1. **Multicast MAC Address Calculation**
   ```cpp
   void EthernetUDP::calculateMulticastMAC(IPAddress ip, uint8_t* mac) {
     // Multicast MAC format: 01:00:5e:XX:XX:XX
     // XX:XX:XX from lower 23 bits of multicast IP
     mac[0] = 0x01;
     mac[1] = 0x00;
     mac[2] = 0x5e;
     mac[3] = ip[1] & 0x7F;  // Clear upper bit for multicast range
     mac[4] = ip[2];
     mac[5] = ip[3];
   }
   ```

2. **Socket Configuration for Multicast**
   ```cpp
   int EthernetUDP::beginMulticast(IPAddress multicast_ip, uint16_t port) {
     if (!isMulticastGroup(multicast_ip)) return 0;
     
     // Configure socket for UDP multicast
     socket(_sock, SnMR::UDP | SnMR::MULTI, port, 0);
     
     // Calculate and set multicast MAC
     uint8_t multicast_mac[6];
     calculateMulticastMAC(multicast_ip, multicast_mac);
     
     // Configure hardware multicast filtering
     configureMulticastSocket(multicast_ip, port);
     return 1;
   }
   ```

3. **Group Management**
   ```cpp
   int EthernetUDP::joinMulticastGroup(IPAddress group_ip) {
     // Note: W5500 doesn't have native IGMP support
     // Manual router configuration or static IGMP snooping required
     
     // Configure socket to receive multicast packets
     uint8_t multicast_mac[6];
     calculateMulticastMAC(group_ip, multicast_mac);
     
     // Set socket to accept packets to multicast MAC
     // Implementation depends on hardware register configuration
     return 1;
   }
   ```

4. **Multicast API Integration**
   ```cpp
   // Example usage:
   EthernetUDP udp;
   IPAddress multicast_group(239, 255, 0, 1);
   
   udp.beginMulticast(multicast_group, 8080);
   udp.joinMulticastGroup(multicast_group);
   
   // Send multicast packet
   udp.beginPacket(multicast_group, 8080);
   udp.write("Hello multicast group!");
   udp.endPacket();
   
   // Receive multicast packets
   int packet_size = udp.parsePacket();
   if (packet_size > 0) {
     // Process received multicast data
   }
   ```

### Automatic Group Management Features

- **IGMP Considerations**: Document W5500's lack of native IGMP support
- **Router Configuration**: Provide guidance for static multicast forwarding
- **Group Lifecycle**: Automatic cleanup on socket close
- **Error Handling**: Proper validation of multicast IP ranges (224.0.0.0/4)

## Directory Structure (Proposed)

```
src/
├── Ethernet3.h                 # Main library header
├── Ethernet3.cpp               # Main EthernetClass implementation
├── EthernetClient.h/cpp         # Client class (updated for multi-instance)
├── EthernetServer.h/cpp         # Server class (updated for multi-instance)
├── EthernetUdp.h/cpp           # UDP class with multicast support (enhanced from EthernetUdp2)
├── hal/
│   ├── EthernetHAL.h           # HAL interface
│   ├── ArduinoHAL.h/cpp        # Arduino HAL implementation
│   ├── ESP32HAL.h/cpp          # ESP32 HAL implementation
│   └── STM32HAL.h/cpp          # STM32 HAL implementation
├── bus/
│   ├── EthernetBus.h           # Bus interface
│   ├── ArduinoSPIBus.h/cpp     # Arduino SPI implementation
│   ├── ESP32SPIBus.h/cpp       # ESP32 SPI implementation
│   └── STM32SPIBus.h/cpp       # STM32 SPI implementation
├── chips/
│   ├── EthernetChip.h          # Chip interface
│   ├── W5500Chip.h/cpp         # W5500 implementation
│   ├── W5100Chip.h/cpp         # W5100 implementation
│   └── utility/                # Chip-specific utilities
│       ├── w5500_registers.h   # W5500 register definitions
│       ├── w5100_registers.h   # W5100 register definitions
│       └── socket.h/cpp        # Common socket operations
└── utility/                    # Backward compatibility
    ├── w5500.h                 # Legacy W5500 interface
    └── socket.h                # Legacy socket interface
```

## Backward Compatibility

### Legacy Support
- Keep existing utility/w5500.h interface for backward compatibility
- Maintain global `Ethernet` instance for existing code
- Ensure all existing examples continue to work without modification

### Migration Path
1. **Phase 1**: New features available alongside existing API
2. **Phase 2**: Deprecation warnings for singleton usage
3. **Phase 3**: New API becomes primary, legacy remains supported

## Testing Strategy

### Unit Tests
- Individual chip driver tests
- HAL implementation validation
- Bus operation verification

### Integration Tests
- Multi-instance scenarios
- Platform-specific testing
- Performance benchmarks

### Platform Testing
- Arduino Uno/Mega/Leonardo
- ESP32/ESP8266
- STM32 variants
- PlatformIO environments

## Implementation Priority

### High Priority (Phase 1)
1. ✅ Create this planning document
2. 🔄 Add `linkActive()` function using existing PHY registers
3. 🔄 Create abstract interfaces (Bus, HAL, Chip)
4. 🔄 Refactor W5500 to use new interfaces
5. 🔄 Restore W5100 support from Ethernet2 heritage

### Medium Priority (Phase 2)
6. 🔄 Implement multi-instance support for all singleton classes
7. 🔄 Complete UDP multicast implementation with automatic group management
8. 🔄 Create PlatformIO configuration
9. 🔄 Add platform-specific implementations

### Lower Priority (Phase 3)
10. 🔄 Enhanced features and optimizations
11. 🔄 Comprehensive testing
12. 🔄 Documentation updates
13. 🔄 Migration guides

## Success Criteria

1. **Functional**: `bool linkActive()` works reliably across supported chips
2. **Multi-Instance**: Can create and use multiple Ethernet instances simultaneously
3. **Platform Support**: Library works on Arduino, ESP32, and STM32 platforms
4. **Backward Compatible**: All existing code continues to work without changes
5. **PlatformIO Ready**: Library installs and builds correctly in PlatformIO
6. **Maintainable**: Clear, documented code structure for future development

## Risk Mitigation

### Breaking Changes
- Extensive testing with existing examples
- Gradual deprecation of old APIs
- Clear migration documentation

### Performance Impact
- Benchmark existing vs new implementation
- Optimize critical paths
- Provide configuration options for resource usage

### Platform Compatibility
- Test on multiple hardware platforms
- Use feature detection and conditional compilation
- Maintain fallback implementations

---

This plan provides a roadmap for modernizing Ethernet3 while maintaining stability and backward compatibility. Implementation will be done incrementally with regular testing and validation.