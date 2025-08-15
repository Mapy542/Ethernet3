# Ethernet3 Library Modernization Plan

## Overview
This document outlines the plan to modernize the Ethernet3 library by adding missing features from Ethernet1, supporting multiple chip instances, and making it platform-agnostic while maintaining compatibility with existing Ethernet2 functionality.

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
- **W5100 Support**: Only supports W5500, missing W5100 compatibility
- **Platform Abstraction**: Hard-coded Arduino SPI dependencies

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
- **Remove Singleton Pattern**: Allow multiple Ethernet instances
- **Bus Abstraction**: Create shared SPI bus management
- **Chip-Specific Classes**: Separate W5500 and W5100 implementations
- **Resource Management**: Proper CS pin and socket allocation per instance

### 3. Platform Abstraction
- **SPI Bus Interface**: Abstract SPI operations for different platforms
- **Platform HAL**: Hardware abstraction layer for timing and GPIO
- **PlatformIO Support**: Add platformio.ini configuration
- **Multi-Platform Testing**: Support Arduino, ESP32, STM32, etc.

### 4. W5100 Support
- **W5100 Driver**: Implement W5100 chip support alongside W5500
- **Unified Interface**: Common API for both chip types
- **Feature Parity**: Ensure both chips support same functionality where possible

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
   - Create W5100Class inheriting from EthernetChip
   - Implement W5100-specific register operations
   - Ensure API compatibility with W5500

3. **Link Status Implementation**
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

2. **Example Usage**
   ```cpp
   // Create bus and HAL instances
   ArduinoSPIBus bus;
   ArduinoHAL hal;
   
   // Create chip instances
   W5500Chip chip1(&bus, &hal, 10); // CS pin 10
   W5500Chip chip2(&bus, &hal, 9);  // CS pin 9
   
   // Create Ethernet instances
   EthernetClass eth1(&chip1);
   EthernetClass eth2(&chip2);
   
   // Initialize
   eth1.begin(mac1, ip1);
   eth2.begin(mac2, ip2);
   
   // Check link status
   if (eth1.linkActive()) {
     // Use eth1...
   }
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

## Directory Structure (Proposed)

```
src/
├── Ethernet3.h                 # Main library header
├── Ethernet3.cpp               # Main EthernetClass implementation
├── EthernetClient.h/cpp         # Client class (updated for multi-instance)
├── EthernetServer.h/cpp         # Server class (updated for multi-instance)
├── EthernetUdp.h/cpp           # UDP class (renamed from EthernetUdp2)
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

### Medium Priority (Phase 2)
5. 🔄 Implement multi-instance support
6. 🔄 Add W5100 chip support
7. 🔄 Create PlatformIO configuration
8. 🔄 Add platform-specific implementations

### Lower Priority (Phase 3)
9. 🔄 Enhanced features and optimizations
10. 🔄 Comprehensive testing
11. 🔄 Documentation updates
12. 🔄 Migration guides

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