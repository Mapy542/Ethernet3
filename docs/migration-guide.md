# Migration Guide

This guide helps you migrate from Ethernet or Ethernet2 libraries to Ethernet3, while maintaining backward compatibility.

## Quick Migration

**Good news!** Existing Ethernet2 code works unchanged with Ethernet3. The library maintains full backward compatibility.

### Immediate Benefits

By simply replacing your library, you get:

- **Link Status Monitoring**: `Ethernet.linkActive()` function
- **Improved Stability**: Better error handling and chip communication
- **Platform Optimizations**: Automatic performance improvements on ESP32/STM32
- **W5100 Support**: If you have W5100 chips, they now work properly

### Zero-Change Migration

Replace your library installation and your existing code continues to work:

```cpp
// This code works exactly the same
#include <Ethernet3.h>  // Changed from <Ethernet.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

void setup() {
  Ethernet.begin(mac);  // Same API
}

void loop() {
  EthernetClient client;  // Same usage
  client.connect("example.com", 80);
}
```

## Gradual Feature Adoption

Once migrated, you can gradually adopt new features:

### 1. Add Link Monitoring

```cpp
void setup() {
  Ethernet.begin(mac);
  
  // NEW: Check physical link status
  if (Ethernet.linkActive()) {
    Serial.println("Ethernet cable connected");
  } else {
    Serial.println("No ethernet cable detected");
  }
}

void loop() {
  // NEW: Monitor link changes
  static bool lastLinkState = false;
  bool currentLink = Ethernet.linkActive();
  
  if (currentLink != lastLinkState) {
    Serial.print("Link ");
    Serial.println(currentLink ? "UP" : "DOWN");
    lastLinkState = currentLink;
  }
}
```

### 2. Add UDP Multicast

```cpp
EthernetUDP udp;
IPAddress multicastGroup(239, 255, 0, 1);

void setup() {
  Ethernet.begin(mac);
  
  // NEW: Start multicast instead of regular UDP
  udp.beginMulticast(multicastGroup, 8080);
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    // Handle multicast packets
    Serial.println("Received multicast packet");
  }
}
```

### 3. Add Multi-Instance Support

```cpp
// NEW: Create multiple Ethernet instances
Ethernet3Class eth1(CHIP_TYPE_W5500, 10);
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);

void setup() {
  eth1.begin(mac1, ip1);
  eth2.begin(mac2, ip2);
  
  // NEW: Create instance-specific clients
  EthernetClient client1(&eth1);
  EthernetClient client2(&eth2);
}
```

## API Compatibility Matrix

| Feature | Ethernet | Ethernet2 | Ethernet3 | Notes |
|---------|----------|-----------|-----------|-------|
| `Ethernet.begin()` | ✓ | ✓ | ✓ | Identical API |
| `EthernetClient` | ✓ | ✓ | ✓ | Identical API |
| `EthernetServer` | ✓ | ✓ | ✓ | Identical API |
| `EthernetUDP` | ✓ | ✓ | ✓ | Identical API |
| `DHCP` | ✓ | ✓ | ✓ | Identical API |
| `linkActive()` | ✗ | ✗ | ✓ | New feature |
| `Multicast` | ✗ | ✗ | ✓ | New feature |
| `Multi-instance` | ✗ | ✗ | ✓ | New feature |
| `W5100 Support` | ✓ | ✓ | ✓ | Restored/improved |
| `W5500 Support` | ✗ | ✓ | ✓ | Enhanced |

## Migration Scenarios

### From Arduino Ethernet Library

**What Changes:**

```cpp
// OLD: Arduino Ethernet
#include <Ethernet.h>
#include <SPI.h>

// NEW: Ethernet3 (SPI included automatically)
#include <Ethernet3.h>
```

**Benefits:**
- W5500 chip support added
- Better error handling
- Link status monitoring
- Multi-instance capability

### From Ethernet2 Library

**What Changes:**

```cpp
// OLD: Ethernet2
#include <Ethernet2.h>

// NEW: Ethernet3
#include <Ethernet3.h>
```

**Benefits:**
- All Ethernet2 features preserved
- Multi-instance architecture added
- UDP multicast support
- Platform optimizations
- Enhanced W5100 support

### From Custom Ethernet Implementations

If you have custom modifications, Ethernet3's architecture makes it easier:

```cpp
// OLD: Modifying global singleton
extern EthernetClass Ethernet;
// Difficult to customize

// NEW: Create custom instances
Ethernet3Class myEthernet(CHIP_TYPE_W5500, customPin);
myEthernet.begin(mac, ip);
```

## Feature Migration Guide

### Socket Management

**Before (Ethernet2):**
```cpp
// Global socket allocation - potential conflicts
EthernetClient client1;
EthernetClient client2;
// Both use same global socket pool
```

**After (Ethernet3):**
```cpp
// Option 1: Backward compatible (same as before)
EthernetClient client1;
EthernetClient client2;

// Option 2: Multi-instance (new capability)
Ethernet3Class eth1(CHIP_TYPE_W5500, 10);
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);

EthernetClient client1(&eth1);  // Uses eth1 sockets
EthernetClient client2(&eth2);  // Uses eth2 sockets
```

### DHCP Configuration

**Before:**
```cpp
if (Ethernet.begin(mac) == 0) {
  Serial.println("DHCP failed");
}
```

**After:**
```cpp
// Same code works, plus enhanced error handling
if (Ethernet.begin(mac) == 0) {
  Serial.println("DHCP failed");
  
  // NEW: Check if it's a link issue
  if (!Ethernet.linkActive()) {
    Serial.println("No ethernet cable connected");
  }
}
```

### UDP Communication

**Before (Ethernet2):**
```cpp
EthernetUDP udp;
udp.begin(8080);

// Send to specific IP
udp.beginPacket(targetIP, targetPort);
udp.write(data, length);
udp.endPacket();
```

**After (Ethernet3):**
```cpp
// Option 1: Same as before (unchanged)
EthernetUDP udp;
udp.begin(8080);

// Option 2: Add multicast capability
IPAddress multicastGroup(239, 255, 0, 1);
udp.beginMulticast(multicastGroup, 8080);

// Send to multicast group
udp.beginPacket(multicastGroup, 8080);
udp.write(data, length);
udp.endPacket();
```

## Platform-Specific Considerations

### Arduino AVR (Uno, Mega, etc.)

**Migration Impact:** None - same performance and behavior

```cpp
// Works exactly the same on Arduino
#include <Ethernet3.h>

void setup() {
  Ethernet.begin(mac);  // Same speed, same behavior
}
```

### ESP32

**Migration Benefits:**
- Automatic 16MHz SPI (was 8MHz)
- FreeRTOS task safety
- Better memory management

```cpp
// OLD: Manual SPI configuration required
#include <Ethernet2.h>
SPIClass SPI(VSPI);
SPI.begin(18, 19, 23, 5);

// NEW: Automatic optimization
#include <Ethernet3.h>
// Automatically uses 16MHz SPI and FreeRTOS features
Ethernet.begin(mac);
```

### STM32

**Migration Benefits:**
- DMA-accelerated SPI transfers
- Hardware timer usage
- Faster GPIO operations

```cpp
// OLD: Generic Arduino compatibility
#include <Ethernet2.h>

// NEW: STM32-optimized automatically
#include <Ethernet3.h>
// Automatically uses DMA and hardware acceleration
```

## Common Migration Issues

### Issue 1: Include Path Changes

**Problem:**
```cpp
#include <Ethernet.h>      // May not exist
#include <Ethernet2.h>     // Old library
```

**Solution:**
```cpp
#include <Ethernet3.h>     // New library
```

### Issue 2: Compilation Errors

**Problem:** "EthernetClass not found"

**Solution:** Ensure old libraries are completely removed:

```bash
# Arduino IDE: Remove old libraries
rm -rf ~/Arduino/libraries/Ethernet/
rm -rf ~/Arduino/libraries/Ethernet2/

# PlatformIO: Clean build
pio lib uninstall Ethernet Ethernet2
pio lib install Ethernet3
```

### Issue 3: Performance Changes

**Problem:** Code runs differently on ESP32/STM32

**Solution:** This is expected - performance improvements are automatic. If you need to disable optimizations:

```cpp
// Force compatibility mode (not recommended)
#define ETHERNET3_COMPATIBILITY_MODE
#include <Ethernet3.h>
```

### Issue 4: Multi-Instance Confusion

**Problem:** Accidentally creating multiple instances

**Solution:** For simple migration, stick to global instance:

```cpp
// AVOID: Unless you specifically need multi-instance
Ethernet3Class eth1(CHIP_TYPE_W5500, 10);

// USE: Global instance for simple migration
Ethernet.begin(mac);  // Uses default global instance
```

## Testing Your Migration

### Validation Checklist

```cpp
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

void setup() {
  Serial.begin(115200);
  
  // Test 1: Basic initialization
  Serial.print("Initializing... ");
  if (Ethernet.begin(mac)) {
    Serial.println("OK");
  } else {
    Serial.println("FAILED");
  }
  
  // Test 2: Link status (new feature)
  Serial.print("Link status... ");
  if (Ethernet.linkActive()) {
    Serial.println("UP");
  } else {
    Serial.println("DOWN");
  }
  
  // Test 3: Network info
  Serial.print("IP: ");
  Serial.println(Ethernet.localIP());
  
  // Test 4: Basic connectivity
  Serial.print("Testing connectivity... ");
  EthernetClient client;
  if (client.connect("httpbin.org", 80)) {
    Serial.println("OK");
    client.stop();
  } else {
    Serial.println("FAILED");
  }
  
  Serial.println("Migration validation complete");
}

void loop() {
  delay(1000);
}
```

### Performance Comparison

```cpp
void benchmarkMigration() {
  Serial.println("Performance Benchmark:");
  
  unsigned long start = micros();
  
  // Test socket operations
  for (int i = 0; i < 100; i++) {
    EthernetClient client;
    client.connect("192.168.1.1", 80);  // Will fail, but tests speed
    client.stop();
  }
  
  unsigned long elapsed = micros() - start;
  Serial.print("100 operations: ");
  Serial.print(elapsed);
  Serial.println(" microseconds");
  
  // Compare with your Ethernet2 baseline
}
```

## Rollback Plan

If you need to rollback to Ethernet2:

### PlatformIO

```ini
[env:myboard]
lib_deps = 
    # Ethernet3  ; Comment out new library
    adafruit/Ethernet2  ; Rollback to Ethernet2
```

### Arduino IDE

1. Remove Ethernet3 library
2. Reinstall Ethernet2 library
3. Change `#include <Ethernet3.h>` back to `#include <Ethernet2.h>`

### Code Changes for Rollback

```cpp
// Remove Ethernet3-specific features
// Ethernet.linkActive();        // Remove - not available in Ethernet2
// udp.beginMulticast(...);      // Remove - not available in Ethernet2

// Keep compatible code
Ethernet.begin(mac);             // Keep - same in both
EthernetClient client;           // Keep - same in both
client.connect(server, port);    // Keep - same in both
```

## Advanced Migration

### Custom Socket Management

**Before (manual socket tracking):**
```cpp
uint8_t available_sockets[MAX_SOCK_NUM];
// Manual tracking required
```

**After (automatic per-instance):**
```cpp
// Automatic socket management per instance
Serial.println(Ethernet.getAvailableSocketCount());
```

### Custom Hardware Configurations

**Before (global modifications):**
```cpp
// Modify library source code for custom pins
```

**After (instance-based configuration):**
```cpp
// Clean instance-based configuration
Ethernet3Class customEth(CHIP_TYPE_W5500, customCSPin);
customEth.begin(mac, ip);
```

## Migration Timeline

### Immediate (Day 1)
1. Replace library installation
2. Change include statements
3. Test basic functionality
4. Verify existing features work

### Short Term (Week 1)
1. Add link status monitoring
2. Test UDP multicast if needed
3. Validate on target hardware
4. Performance testing

### Long Term (Month 1)
1. Consider multi-instance architecture
2. Platform-specific optimizations
3. Advanced features adoption
4. Documentation updates

Your existing code continues to work while you gradually adopt new capabilities at your own pace.

## Support

- **GitHub Issues**: Report migration problems
- **Examples**: See migration-specific examples
- **Documentation**: Complete API reference available
- **Community**: Share migration experiences