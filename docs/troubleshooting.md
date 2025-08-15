# Troubleshooting

This guide helps you diagnose and fix common issues with the Ethernet3 library.

## Quick Diagnostic

Start with this diagnostic script to identify common issues:

```cpp
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) ; // Wait for serial
  
  Serial.println("=== Ethernet3 Diagnostic ===");
  
  // Test 1: Library compilation
  Serial.println("✓ Library compiled successfully");
  
  // Test 2: Hardware detection
  Serial.print("Platform: ");
#ifdef ESP32
  Serial.println("ESP32");
#elif defined(STM32F4)
  Serial.println("STM32F4");
#elif defined(STM32F1)
  Serial.println("STM32F1");
#elif defined(ARDUINO_ARCH_AVR)
  Serial.println("Arduino AVR");
#else
  Serial.println("Generic");
#endif
  
  // Test 3: Ethernet initialization
  Serial.print("Initializing Ethernet... ");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("FAILED - Trying static IP");
    IPAddress ip(192, 168, 1, 177);
    Ethernet.begin(mac, ip);
    Serial.print("Static IP set to: ");
    Serial.println(Ethernet.localIP());
  } else {
    Serial.println("SUCCESS");
    Serial.print("DHCP IP: ");
    Serial.println(Ethernet.localIP());
  }
  
  // Test 4: Link status
  Serial.print("Physical link: ");
  if (Ethernet.linkActive()) {
    Serial.println("ACTIVE");
  } else {
    Serial.println("INACTIVE - Check cable and connections");
  }
  
  // Test 5: Basic connectivity
  Serial.print("Testing connectivity... ");
  EthernetClient client;
  if (client.connect("8.8.8.8", 53)) {  // Google DNS
    Serial.println("SUCCESS");
    client.stop();
  } else {
    Serial.println("FAILED - Check network configuration");
  }
  
  Serial.println("=== Diagnostic Complete ===");
}

void loop() {
  // Monitor link status changes
  static bool lastLinkState = false;
  bool currentLinkState = Ethernet.linkActive();
  
  if (currentLinkState != lastLinkState) {
    Serial.print("Link status changed: ");
    Serial.println(currentLinkState ? "UP" : "DOWN");
    lastLinkState = currentLinkState;
  }
  
  delay(1000);
}
```

## Common Issues

### 1. Compilation Errors

#### "Ethernet3.h: No such file or directory"

**Cause:** Library not installed or wrong include path

**Solutions:**

**Arduino IDE:**
```cpp
// Make sure you have the correct include
#include <Ethernet3.h>  // NOT <Ethernet.h> or <Ethernet2.h>
```

**PlatformIO:**
```ini
[env:myboard]
lib_deps = 
    Ethernet3  ; Make sure this is in your platformio.ini
```

**Manual Installation:**
- Download from GitHub releases
- Extract to `Arduino/libraries/Ethernet3/`
- Restart Arduino IDE

#### "Multiple definition of 'Ethernet'"

**Cause:** Multiple Ethernet libraries installed

**Solution:**
1. Remove old libraries:
   ```bash
   # Arduino libraries directory
   rm -rf ~/Arduino/libraries/Ethernet/
   rm -rf ~/Arduino/libraries/Ethernet2/
   ```

2. PlatformIO:
   ```bash
   pio lib uninstall Ethernet Ethernet2
   pio lib install Ethernet3
   ```

#### "EthernetClass was not declared in this scope"

**Cause:** Conflicting library versions

**Solution:**
```cpp
// Use the new class name explicitly
Ethernet3Class eth(CHIP_TYPE_W5500, 10);

// Or use the global instance
extern Ethernet3Class Ethernet;  // This is provided by the library
```

### 2. Network Initialization Issues

#### DHCP Fails

**Symptoms:**
- `Ethernet.begin(mac)` returns 0
- No IP address assigned
- localIP() returns 0.0.0.0

**Diagnostic:**
```cpp
void diagnoseDHCP() {
  Serial.println("DHCP Diagnostic:");
  
  // Check link first
  if (!Ethernet.linkActive()) {
    Serial.println("❌ No physical link - check cable");
    return;
  }
  
  Serial.println("✓ Physical link active");
  
  // Try DHCP with timeout
  Serial.print("Attempting DHCP... ");
  unsigned long start = millis();
  if (Ethernet.begin(mac) == 0) {
    Serial.print("FAILED after ");
    Serial.print(millis() - start);
    Serial.println("ms");
    
    // Try static IP
    IPAddress ip(192, 168, 1, 177);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    
    Ethernet.begin(mac, ip, gateway, subnet);
    Serial.print("Using static IP: ");
    Serial.println(Ethernet.localIP());
  } else {
    Serial.print("SUCCESS after ");
    Serial.print(millis() - start);
    Serial.println("ms");
    
    Serial.print("Assigned IP: ");
    Serial.println(Ethernet.localIP());
    Serial.print("Gateway: ");
    Serial.println(Ethernet.gatewayIP());
    Serial.print("Subnet: ");
    Serial.println(Ethernet.subnetMask());
    Serial.print("DNS: ");
    Serial.println(Ethernet.dnsServerIP());
  }
}
```

**Solutions:**
1. **Check Physical Connection:**
   ```cpp
   if (!Ethernet.linkActive()) {
     Serial.println("Check Ethernet cable and connections");
   }
   ```

2. **Try Static IP:**
   ```cpp
   IPAddress ip(192, 168, 1, 177);
   IPAddress gateway(192, 168, 1, 1);
   IPAddress subnet(255, 255, 255, 0);
   IPAddress dns(8, 8, 8, 8);
   
   Ethernet.begin(mac, ip, dns, gateway, subnet);
   ```

3. **Increase DHCP Timeout:**
   ```cpp
   // Custom DHCP client with longer timeout
   DhcpClass dhcp;
   if (dhcp.beginWithDHCP(mac, 120000) == 1) {  // 2 minute timeout
     Serial.println("DHCP successful with extended timeout");
   }
   ```

#### No Link Detected

**Symptoms:**
- `Ethernet.linkActive()` returns false
- Network operations fail immediately

**Causes & Solutions:**

1. **Cable Issues:**
   - Try different Ethernet cable
   - Check cable integrity
   - Ensure cable is fully inserted

2. **Power Issues:**
   ```cpp
   void checkPower() {
     Serial.println("Power Diagnostic:");
     
     // Check if module responds to SPI
     // This is a low-level test
     Serial.print("SPI Communication: ");
     // Implementation depends on chip type
     if (testSPICommunication()) {
       Serial.println("✓ OK");
     } else {
       Serial.println("❌ FAILED - Check power and connections");
     }
   }
   ```

3. **SPI Wiring:**
   ```cpp
   void checkSPIWiring() {
     Serial.println("SPI Wiring Check:");
     Serial.println("Expected connections:");
     Serial.println("MOSI -> Pin 11 (or SPI MOSI)");
     Serial.println("MISO -> Pin 12 (or SPI MISO)");
     Serial.println("SCK  -> Pin 13 (or SPI SCK)");
     Serial.println("CS   -> Pin 10 (or custom)");
     Serial.println("VCC  -> 3.3V or 5V");
     Serial.println("GND  -> GND");
   }
   ```

### 3. Connection Issues

#### Client Connections Fail

**Symptoms:**
- `client.connect()` returns 0
- Connections timeout immediately

**Diagnostic:**
```cpp
void diagnoseConnections() {
  Serial.println("Connection Diagnostic:");
  
  // Test 1: Local network connectivity
  Serial.print("Testing local gateway... ");
  EthernetClient client;
  if (client.connect(Ethernet.gatewayIP(), 80)) {
    Serial.println("✓ Can reach gateway");
    client.stop();
  } else {
    Serial.println("❌ Cannot reach gateway");
  }
  
  // Test 2: DNS resolution
  Serial.print("Testing DNS (Google)... ");
  if (client.connect("8.8.8.8", 53)) {
    Serial.println("✓ Can reach external IP");
    client.stop();
  } else {
    Serial.println("❌ Cannot reach external IP");
  }
  
  // Test 3: HTTP connectivity
  Serial.print("Testing HTTP (httpbin.org)... ");
  if (client.connect("httpbin.org", 80)) {
    Serial.println("✓ Can reach HTTP server");
    client.stop();
  } else {
    Serial.println("❌ Cannot reach HTTP server");
  }
}
```

**Solutions:**

1. **Check Socket Availability:**
   ```cpp
   void checkSockets() {
     Serial.print("Available sockets: ");
     Serial.println(Ethernet.getAvailableSocketCount());
     
     if (Ethernet.getAvailableSocketCount() == 0) {
       Serial.println("❌ No sockets available");
       Serial.println("Close existing connections or use multi-instance");
     }
   }
   ```

2. **Firewall/Router Issues:**
   ```cpp
   // Test with different ports
   int testPorts[] = {80, 443, 8080, 53};
   for (int i = 0; i < 4; i++) {
     EthernetClient client;
     Serial.print("Testing port ");
     Serial.print(testPorts[i]);
     Serial.print("... ");
     
     if (client.connect("httpbin.org", testPorts[i])) {
       Serial.println("✓ OPEN");
       client.stop();
     } else {
       Serial.println("❌ BLOCKED");
     }
   }
   ```

3. **DNS Issues:**
   ```cpp
   // Try IP address instead of hostname
   EthernetClient client;
   
   // Instead of: client.connect("example.com", 80);
   // Use IP:
   IPAddress serverIP(93, 184, 216, 34);  // example.com IP
   if (client.connect(serverIP, 80)) {
     Serial.println("✓ IP connection works - DNS issue");
   }
   ```

### 4. Server Issues

#### Server Not Accepting Connections

**Symptoms:**
- `server.available()` never returns a client
- Connections refused or timeout

**Diagnostic:**
```cpp
EthernetServer server(80);

void diagnoseServer() {
  Serial.println("Server Diagnostic:");
  
  server.begin();
  
  // Check if server started
  Serial.print("Server listening on port 80 at ");
  Serial.println(Ethernet.localIP());
  
  // Test local connection
  Serial.print("Testing local connection... ");
  EthernetClient testClient;
  if (testClient.connect(Ethernet.localIP(), 80)) {
    Serial.println("✓ Server accepting connections");
    testClient.stop();
  } else {
    Serial.println("❌ Server not accepting connections");
  }
}
```

**Solutions:**

1. **Port Conflicts:**
   ```cpp
   // Try different port
   EthernetServer server(8080);  // Instead of 80
   server.begin();
   Serial.print("Server started on port 8080 at ");
   Serial.println(Ethernet.localIP());
   ```

2. **Socket Exhaustion:**
   ```cpp
   void handleClients() {
     EthernetClient client = server.available();
     if (client) {
       // Handle client quickly
       handleClientFast(client);
       
       // Always close promptly
       client.stop();
       
       Serial.print("Sockets remaining: ");
       Serial.println(Ethernet.getAvailableSocketCount());
     }
   }
   ```

### 5. UDP/Multicast Issues

#### UDP Packets Not Received

**Symptoms:**
- `udp.parsePacket()` always returns 0
- Multicast groups not receiving packets

**Diagnostic:**
```cpp
EthernetUDP udp;
IPAddress multicastGroup(239, 255, 0, 1);

void diagnoseUDP() {
  Serial.println("UDP Diagnostic:");
  
  // Test 1: Basic UDP
  Serial.print("Starting UDP on port 8080... ");
  if (udp.begin(8080)) {
    Serial.println("✓ SUCCESS");
  } else {
    Serial.println("❌ FAILED");
    return;
  }
  
  // Test 2: Multicast
  Serial.print("Joining multicast group... ");
  if (udp.beginMulticast(multicastGroup, 8080)) {
    Serial.println("✓ SUCCESS");
    Serial.print("Listening on ");
    Serial.print(multicastGroup);
    Serial.println(":8080");
  } else {
    Serial.println("❌ FAILED");
  }
  
  // Test 3: Send to self
  Serial.print("Testing loopback... ");
  udp.beginPacket(Ethernet.localIP(), 8080);
  udp.print("Test packet");
  if (udp.endPacket()) {
    Serial.println("✓ Packet sent");
    
    delay(100);
    if (udp.parsePacket()) {
      Serial.println("✓ Packet received");
    } else {
      Serial.println("❌ Packet not received");
    }
  } else {
    Serial.println("❌ Send failed");
  }
}
```

**Solutions:**

1. **Router Configuration:**
   ```cpp
   // For multicast, router must forward multicast packets
   Serial.println("Multicast Requirements:");
   Serial.println("- Router must support IGMP");
   Serial.println("- IGMP snooping may need configuration");
   Serial.println("- Try static multicast routes");
   ```

2. **Firewall Issues:**
   ```cpp
   // Test with broadcast first
   IPAddress broadcast = Ethernet.localIP();
   broadcast[3] = 255;  // Set last octet to 255
   
   udp.beginPacket(broadcast, 8080);
   udp.print("Broadcast test");
   udp.endPacket();
   ```

### 6. Multi-Instance Issues

#### Instance Conflicts

**Symptoms:**
- Only one instance works
- SPI conflicts between instances
- Socket allocation errors

**Diagnostic:**
```cpp
Ethernet3Class eth1(CHIP_TYPE_W5500, 10);
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);

void diagnoseMultiInstance() {
  Serial.println("Multi-Instance Diagnostic:");
  
  // Test instance 1
  Serial.print("Initializing eth1 (W5500, pin 10)... ");
  if (eth1.begin(mac1, ip1)) {
    Serial.println("✓ SUCCESS");
    Serial.print("  IP: ");
    Serial.println(eth1.localIP());
    Serial.print("  Link: ");
    Serial.println(eth1.linkActive() ? "UP" : "DOWN");
  } else {
    Serial.println("❌ FAILED");
  }
  
  // Test instance 2
  Serial.print("Initializing eth2 (W5100, pin 9)... ");
  if (eth2.begin(mac2, ip2)) {
    Serial.println("✓ SUCCESS");
    Serial.print("  IP: ");
    Serial.println(eth2.localIP());
    Serial.print("  Link: ");
    Serial.println(eth2.linkActive() ? "UP" : "DOWN");
  } else {
    Serial.println("❌ FAILED");
  }
  
  // Test independence
  Serial.println("Testing instance independence...");
  EthernetClient client1(&eth1);
  EthernetClient client2(&eth2);
  
  bool conn1 = client1.connect("httpbin.org", 80);
  bool conn2 = client2.connect("example.com", 80);
  
  Serial.print("Client1 connection: ");
  Serial.println(conn1 ? "✓ SUCCESS" : "❌ FAILED");
  Serial.print("Client2 connection: ");
  Serial.println(conn2 ? "✓ SUCCESS" : "❌ FAILED");
  
  if (conn1) client1.stop();
  if (conn2) client2.stop();
}
```

**Solutions:**

1. **Check Pin Assignments:**
   ```cpp
   // Ensure different CS pins
   Ethernet3Class eth1(CHIP_TYPE_W5500, 10);  // CS pin 10
   Ethernet3Class eth2(CHIP_TYPE_W5100, 9);   // CS pin 9
   
   // Pins must not conflict with SPI pins (11, 12, 13 on Arduino)
   ```

2. **MAC Address Conflicts:**
   ```cpp
   // Each instance needs unique MAC
   byte mac1[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01};
   byte mac2[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x02};  // Different last byte
   ```

3. **IP Address Conflicts:**
   ```cpp
   // Use different IP addresses or subnets
   IPAddress ip1(192, 168, 1, 177);
   IPAddress ip2(192, 168, 1, 178);  // Different IP
   
   // Or different subnets
   IPAddress ip1(192, 168, 1, 177);
   IPAddress ip2(10, 0, 0, 177);     // Different subnet
   ```

### 7. Platform-Specific Issues

#### ESP32 Issues

**Problem:** Random crashes or watchdog timeouts

**Solution:**
```cpp
// Increase task stack sizes
void networkTask(void* parameter) {
  // Long-running network operations
  while (true) {
    handleNetworking();
    vTaskDelay(pdMS_TO_TICKS(10));  // Yield to other tasks
  }
}

void setup() {
  // Create task with larger stack
  xTaskCreate(networkTask, "Network", 8192, NULL, 1, NULL);  // 8KB stack
}
```

**Problem:** SPI conflicts with WiFi

**Solution:**
```cpp
// Use different SPI instance
#include <driver/spi_master.h>

void setup() {
  // Configure Ethernet to use HSPI instead of VSPI
  // This leaves VSPI available for WiFi
  SPI.begin(14, 12, 13, 15);  // HSPI pins
}
```

#### STM32 Issues

**Problem:** SPI speed too high

**Solution:**
```cpp
// Reduce SPI speed for stability
#define ETHERNET3_MAX_SPI_SPEED 8000000  // 8MHz instead of 12MHz

// Or use compatibility mode
#define ETHERNET3_CONSERVATIVE_SPI
#include <Ethernet3.h>
```

#### Arduino AVR Issues

**Problem:** Out of memory

**Solution:**
```cpp
// Reduce buffer sizes
#define ETHERNET3_SOCKET_BUFFER_SIZE 512  // Smaller buffers

// Use PROGMEM for constants
const char response[] PROGMEM = "HTTP/1.1 200 OK\r\n";

// Minimize global variables
void useStackInstead() {
  char buffer[64];  // Stack allocation instead of global
  // Use buffer...
}
```

## Performance Issues

### Slow Network Performance

**Diagnostic:**
```cpp
void performanceTest() {
  Serial.println("Performance Test:");
  
  unsigned long start = millis();
  
  EthernetClient client;
  if (client.connect("httpbin.org", 80)) {
    client.println("GET /bytes/1024 HTTP/1.1");
    client.println("Host: httpbin.org");
    client.println("Connection: close");
    client.println();
    
    int totalBytes = 0;
    while (client.connected()) {
      while (client.available()) {
        client.read();
        totalBytes++;
      }
    }
    
    unsigned long elapsed = millis() - start;
    Serial.print("Downloaded ");
    Serial.print(totalBytes);
    Serial.print(" bytes in ");
    Serial.print(elapsed);
    Serial.println("ms");
    
    float kbps = (totalBytes / 1024.0) / (elapsed / 1000.0);
    Serial.print("Speed: ");
    Serial.print(kbps, 2);
    Serial.println(" KB/s");
  }
  
  client.stop();
}
```

**Solutions:**

1. **Platform Optimizations:**
   ```cpp
   // Enable platform-specific optimizations
   #define ETHERNET3_HIGH_PERFORMANCE
   #include <Ethernet3.h>
   ```

2. **Buffer Tuning:**
   ```cpp
   // Increase buffer sizes for throughput
   #define ETHERNET3_SOCKET_BUFFER_SIZE 2048
   
   // Or decrease for memory-constrained platforms
   #define ETHERNET3_SOCKET_BUFFER_SIZE 256
   ```

3. **SPI Speed Optimization:**
   ```cpp
   // Test maximum stable SPI speed
   void testSPISpeed() {
     uint32_t speeds[] = {4000000, 8000000, 12000000, 16000000};
     
     for (int i = 0; i < 4; i++) {
       Serial.print("Testing ");
       Serial.print(speeds[i] / 1000000);
       Serial.print("MHz... ");
       
       if (testEthernetAtSpeed(speeds[i])) {
         Serial.println("✓ STABLE");
       } else {
         Serial.println("❌ UNSTABLE");
         break;
       }
     }
   }
   ```

## Getting Help

### Collect Debug Information

When reporting issues, include this information:

```cpp
void collectDebugInfo() {
  Serial.println("=== DEBUG INFORMATION ===");
  
  // Platform info
  Serial.print("Platform: ");
#ifdef ESP32
  Serial.println("ESP32");
  Serial.print("ESP32 Chip Rev: ");
  Serial.println(ESP.getChipRevision());
  Serial.print("Free Heap: ");
  Serial.println(ESP.getFreeHeap());
#elif defined(ARDUINO_ARCH_AVR)
  Serial.println("Arduino AVR");
  Serial.print("Free RAM: ");
  Serial.println(freeRam());
#else
  Serial.println("Other");
#endif
  
  // Library version
  Serial.println("Library: Ethernet3 v1.6.0");
  
  // Network status
  Serial.print("IP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("Link: ");
  Serial.println(Ethernet.linkActive() ? "UP" : "DOWN");
  Serial.print("Available sockets: ");
  Serial.println(Ethernet.getAvailableSocketCount());
  
  // Hardware info
  Serial.println("Expected wiring:");
  Serial.println("CS -> Pin 10, MOSI -> 11, MISO -> 12, SCK -> 13");
  
  Serial.println("=== END DEBUG INFO ===");
}

#ifdef ARDUINO_ARCH_AVR
int freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
#endif
```

### Where to Get Help

1. **GitHub Issues**: [https://github.com/Mapy542/Ethernet3/issues](https://github.com/Mapy542/Ethernet3/issues)
2. **Documentation**: Complete guides in `docs/` folder
3. **Examples**: Working code samples in `examples/` folder
4. **Community Forums**: Arduino, PlatformIO communities

### Before Reporting Issues

1. **Run Diagnostic Script**: Use the diagnostic code above
2. **Check Examples**: Verify examples work on your hardware
3. **Test Basic Functionality**: Ensure physical connections work
4. **Search Existing Issues**: Check if the problem is already reported
5. **Collect Debug Information**: Include the debug output when reporting

Remember: Most issues are related to hardware connections, power supply, or network configuration rather than library bugs.