# Unified Platform Architecture

The Ethernet3 library now uses a **unified platform architecture** that combines hardware abstraction and SPI communication into a single, coherent interface.

## What Changed

### Before: Separated HAL and BUS
The previous implementation had separate abstractions:
- **HAL (Hardware Abstraction Layer)**: General platform functions (delay, GPIO, etc.)
- **BUS**: SPI communication specific functions

This required users to understand two different abstractions and manage them separately:

```cpp
// Old approach - separated concerns
ArduinoHAL hal;
ArduinoSPIBus bus;
Ethernet3Class eth(CHIP_TYPE_W5500, 10, &bus, &hal);
```

### After: Unified Platform
The new implementation combines both concerns into a single **EthernetPlatform** interface:

```cpp
// New approach - unified platform
ArduinoPlatform platform;
Ethernet3Class eth(CHIP_TYPE_W5500, 10, &platform);

// Or simply use the default
Ethernet3Class eth(CHIP_TYPE_W5500, 10);  // Uses ArduinoPlatform automatically
```

## Benefits

1. **Simpler API**: One interface instead of two
2. **Easier to understand**: Single abstraction layer
3. **Less memory overhead**: Eliminates duplicate capability tracking
4. **Cleaner code**: No need to manage two separate objects
5. **Better maintainability**: Single implementation per platform

## Platform Implementations

### ArduinoPlatform
- Standard Arduino boards (Uno, Mega, Leonardo)
- ESP8266, ESP32 when using Arduino framework
- Default 8MHz SPI, can be configured

### ESP32Platform
- ESP32-specific optimizations
- FreeRTOS task scheduling support
- 16MHz SPI by default
- Dual-core awareness with `getCoreId()`
- Critical section support

### STM32Platform
- STM32F1, STM32F4 support
- DMA transfer optimizations
- 12MHz SPI by default for STM32F4
- Hardware acceleration features

## Migration Guide

### For Library Users
If you were using the default constructors, **no changes needed**:
```cpp
// This still works exactly the same
Ethernet3Class eth(CHIP_TYPE_W5500, 10);
```

### For Advanced Users
If you were using custom HAL/BUS instances:
```cpp
// Old way
ESP32HAL hal;
ESP32SPIBus bus;
Ethernet3Class eth(CHIP_TYPE_W5500, 10, &bus, &hal);

// New way
ESP32Platform platform;
Ethernet3Class eth(CHIP_TYPE_W5500, 10, &platform);
```

## Implementation Details

The `EthernetPlatform` base class provides:

**Hardware Functions:**
- `delay(ms)`, `millis()`, `yield()`
- `digitalWrite()`, `pinMode()`, `digitalRead()`

**SPI Functions:**
- `spiBegin()`, `spiEnd()`, `spiTransfer()`
- `spiSetBitOrder()`, `spiSetDataMode()`, `spiSetClockDivider()`
- `spiBeginTransaction()`, `spiEndTransaction()`

**Platform Capabilities:**
- `getPlatformName()`, `getPlatformInfo()`
- `hasHardwareAcceleration()`, `supportsDMA()`, `supportsRealTime()`
- `supportsTransactions()`, `getMaxSPISpeed()`

This unified approach eliminates the artificial separation between general hardware and SPI communication while maintaining all platform-specific optimizations.