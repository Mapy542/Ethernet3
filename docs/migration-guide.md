# Migration Guide: From Arduino Ethernet to Ethernet3

This guide helps you migrate existing Arduino Ethernet library code to the enhanced Ethernet3 library.

## Table of Contents

1. [Quick Migration Checklist](#quick-migration-checklist)
2. [Basic Setup Changes](#basic-setup-changes)
3. [Class and Method Changes](#class-and-method-changes)
4. [Common Migration Patterns](#common-migration-patterns)
5. [New Features to Consider](#new-features-to-consider)
6. [Troubleshooting Migration Issues](#troubleshooting-migration-issues)

## Quick Migration Checklist

- [ ] Update include statements
- [ ] Replace singleton `Ethernet` with `EthernetClass` instance
- [ ] Add chip interface (W5500, W5100, etc.)
- [ ] Update client/server instantiation
- [ ] Update UDP class name (EthernetUDP → EthernetUDP)
- [ ] Test basic functionality
- [ ] Consider new features (multicast, enhanced DHCP, etc.)

## Basic Setup Changes

### Include Statements

**Before (Arduino Ethernet):**
```cpp
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
```

**After (Ethernet3):**
```cpp
#include <SPI.h>
#include <Ethernet3.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp2.h>
#include <chips/w5500.h>  // or appropriate chip
```

### Basic Initialization

**Before (Arduino Ethernet):**
```cpp
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

void setup() {
    // Direct use of global singleton
    if (Ethernet.begin(mac) == 0) {
        Serial.println("Failed to configure Ethernet using DHCP");
        // Fallback to static IP
        IPAddress ip(192, 168, 1, 177);
        Ethernet.begin(mac, ip);
    }
    
    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());
}
```

**After (Ethernet3):**
```cpp
#include <chips/w5500.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// Create chip interface and Ethernet instance
W5500 chip(10);  // CS pin 10
EthernetClass ethernet(&chip);

void setup() {
    // Use instance instead of singleton
    if (ethernet.begin(mac) == 0) {
        Serial.println("Failed to configure Ethernet using DHCP");
        // Fallback to static IP
        IPAddress ip(192, 168, 1, 177);
        ethernet.begin(mac, ip);
    }
    
    Serial.print("My IP address: ");
    Serial.println(ethernet.localIP());
}
```

## Class and Method Changes

### EthernetClient

**Before (Arduino Ethernet):**
```cpp
EthernetClient client;

void setup() {
    // ... ethernet initialization
}

void loop() {
    if (client.connect("example.com", 80)) {
        client.println("GET / HTTP/1.1");
        client.println("Host: example.com");
        client.println("Connection: close");
        client.println();
    }
}
```

**After (Ethernet3):**
```cpp
EthernetClient client(&ethernet, &chip);

void setup() {
    // ... ethernet initialization
}

void loop() {
    if (client.connect("example.com", 80)) {
        client.println("GET / HTTP/1.1");
        client.println("Host: example.com");
        client.println("Connection: close");
        client.println();
    }
}
```

### EthernetServer

**Before (Arduino Ethernet):**
```cpp
EthernetServer server(80);

void setup() {
    // ... ethernet initialization
    server.begin();
}

void loop() {
    EthernetClient client = server.available();
    if (client) {
        // Handle client
    }
}
```

**After (Ethernet3):**
```cpp
EthernetServer server(&ethernet, &chip, 80);

void setup() {
    // ... ethernet initialization
    server.begin();
}

void loop() {
    EthernetClient client = server.available();
    if (client) {
        // Handle client
    }
}
```

### EthernetUDP

**Before (Arduino Ethernet):**
```cpp
EthernetUDP udp;

void setup() {
    // ... ethernet initialization
    udp.begin(8888);
}

void loop() {
    int packetSize = udp.parsePacket();
    if (packetSize) {
        // Handle packet
    }
}
```

**After (Ethernet3):**
```cpp
EthernetUDP udp(&ethernet, &chip);

void setup() {
    // ... ethernet initialization
    udp.begin(8888);
}

void loop() {
    int packetSize = udp.parsePacket();
    if (packetSize) {
        // Handle packet
    }
}
```

## Common Migration Patterns

### Pattern 1: Simple Web Client

**Before:**
```cpp
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress server(93, 184, 216, 34);  // Google
EthernetClient client;

void setup() {
    Serial.begin(9600);
    if (Ethernet.begin(mac) == 0) {
        Serial.println("Failed to configure Ethernet using DHCP");
        for(;;);
    }
    delay(1000);
    Serial.println("connecting...");

    if (client.connect(server, 80)) {
        Serial.println("connected");
        client.println("GET /search?q=arduino HTTP/1.1");
        client.println("Host: www.google.com");
        client.println("Connection: close");
        client.println();
    } else {
        Serial.println("connection failed");
    }
}

void loop() {
    if (client.available()) {
        char c = client.read();
        Serial.print(c);
    }
    
    if (!client.connected()) {
        Serial.println();
        Serial.println("disconnecting.");
        client.stop();
        for(;;);
    }
}
```

**After:**
```cpp
#include <SPI.h>
#include <Ethernet3.h>
#include <chips/w5500.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress server(93, 184, 216, 34);  // Google

W5500 chip(10);
EthernetClass ethernet(&chip);
EthernetClient client(&ethernet, &chip);

void setup() {
    Serial.begin(9600);
    if (ethernet.begin(mac) == 0) {
        Serial.println("Failed to configure Ethernet using DHCP");
        for(;;);
    }
    delay(1000);
    Serial.println("connecting...");

    if (client.connect(server, 80)) {
        Serial.println("connected");
        client.println("GET /search?q=arduino HTTP/1.1");
        client.println("Host: www.google.com");
        client.println("Connection: close");
        client.println();
    } else {
        Serial.println("connection failed");
    }
}

void loop() {
    if (client.available()) {
        char c = client.read();
        Serial.print(c);
    }
    
    if (!client.connected()) {
        Serial.println();
        Serial.println("disconnecting.");
        client.stop();
        for(;;);
    }
    
    // Enhanced: maintain DHCP lease
    ethernet.maintain();
}
```

### Pattern 2: Web Server

**Before:**
```cpp
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);
EthernetServer server(80);

void setup() {
    Serial.begin(9600);
    Ethernet.begin(mac, ip);
    server.begin();
    Serial.print("Server is at ");
    Serial.println(Ethernet.localIP());
}

void loop() {
    EthernetClient client = server.available();
    if (client) {
        Serial.println("new client");
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                Serial.write(c);
                if (c == '\n' && currentLineIsBlank) {
                    // Send response
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println();
                    client.println("<!DOCTYPE HTML>");
                    client.println("<html>");
                    client.println("<h1>Hello World!</h1>");
                    client.println("</html>");
                    break;
                }
                if (c == '\n') {
                    currentLineIsBlank = true;
                } else if (c != '\r') {
                    currentLineIsBlank = false;
                }
            }
        }
        delay(1);
        client.stop();
        Serial.println("client disconnected");
    }
}
```

**After:**
```cpp
#include <SPI.h>
#include <Ethernet3.h>
#include <chips/w5500.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);

W5500 chip(10);
EthernetClass ethernet(&chip);
EthernetServer server(&ethernet, &chip, 80);

void setup() {
    Serial.begin(9600);
    ethernet.begin(mac, ip);
    server.begin();
    Serial.print("Server is at ");
    Serial.println(ethernet.localIP());
}

void loop() {
    EthernetClient client = server.available();
    if (client) {
        Serial.println("new client");
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                Serial.write(c);
                if (c == '\n' && currentLineIsBlank) {
                    // Send response
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println();
                    client.println("<!DOCTYPE HTML>");
                    client.println("<html>");
                    client.println("<h1>Hello World!</h1>");
                    client.println("</html>");
                    break;
                }
                if (c == '\n') {
                    currentLineIsBlank = true;
                } else if (c != '\r') {
                    currentLineIsBlank = false;
                }
            }
        }
        delay(1);
        client.stop();
        Serial.println("client disconnected");
    }
    
    // Enhanced: maintain DHCP lease if using DHCP
    ethernet.maintain();
}
```

### Pattern 3: UDP Communication

**Before:**
```cpp
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
unsigned int localPort = 8888;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
EthernetUDP Udp;

void setup() {
    Serial.begin(9600);
    Ethernet.begin(mac);
    Udp.begin(localPort);
    Serial.print("Local IP: ");
    Serial.println(Ethernet.localIP());
}

void loop() {
    int packetSize = Udp.parsePacket();
    if (packetSize) {
        Serial.print("Received packet of size ");
        Serial.println(packetSize);
        Serial.print("From ");
        IPAddress remote = Udp.remoteIP();
        for (int i =0; i < 4; i++) {
            Serial.print(remote[i], DEC);
            if (i < 3) {
                Serial.print(".");
            }
        }
        Serial.print(", port ");
        Serial.println(Udp.remotePort());

        Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
        Serial.println("Contents:");
        Serial.println(packetBuffer);
    }
}
```

**After:**
```cpp
#include <SPI.h>
#include <Ethernet3.h>
#include <EthernetUdp2.h>
#include <chips/w5500.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
unsigned int localPort = 8888;
char packetBuffer[512];  // Reasonable buffer size

W5500 chip(10);
EthernetClass ethernet(&chip);
EthernetUDP Udp(&ethernet, &chip);

void setup() {
    Serial.begin(9600);
    ethernet.begin(mac);
    Udp.begin(localPort);
    Serial.print("Local IP: ");
    Serial.println(ethernet.localIP());
}

void loop() {
    int packetSize = Udp.parsePacket();
    if (packetSize) {
        Serial.print("Received packet of size ");
        Serial.println(packetSize);
        Serial.print("From ");
        IPAddress remote = Udp.remoteIP();
        for (int i = 0; i < 4; i++) {
            Serial.print(remote[i], DEC);
            if (i < 3) {
                Serial.print(".");
            }
        }
        Serial.print(", port ");
        Serial.println(Udp.remotePort());

        int len = Udp.read(packetBuffer, sizeof(packetBuffer) - 1);
        packetBuffer[len] = 0;  // Null terminate
        Serial.println("Contents:");
        Serial.println(packetBuffer);
    }
    
    // Enhanced: maintain DHCP lease
    ethernet.maintain();
}
```

## New Features to Consider

### 1. Enhanced DHCP Monitoring

```cpp
void loop() {
    // Monitor DHCP status
    int dhcpStatus = ethernet.maintain();
    switch (dhcpStatus) {
        case DHCP_CHECK_RENEW_OK:
            Serial.println("DHCP lease renewed");
            break;
        case DHCP_CHECK_REBIND_OK:
            Serial.println("DHCP lease rebound");
            break;
        case DHCP_CHECK_RENEW_FAIL:
            Serial.println("DHCP lease renewal failed");
            break;
        case DHCP_CHECK_REBIND_FAIL:
            Serial.println("DHCP lease rebind failed");
            break;
    }
}
```

### 2. Multicast UDP

```cpp
void setup() {
    // ... basic setup
    
    // Join multicast group
    IPAddress multicastIP(224, 1, 1, 1);
    Udp.beginMulticast(multicastIP, 1234);
    
    // Can join additional groups
    Udp.joinMulticastGroup(IPAddress(224, 2, 2, 2));
}

void loop() {
    // Handle multicast packets normally
    int packetSize = Udp.parsePacket();
    if (packetSize) {
        // Process multicast packet
    }
}
```

### 3. Better Connection Status

```cpp
void loop() {
    if (client.connected()) {
        // Enhanced status checking
        uint8_t status = client.status();
        if (status == SnSR::CLOSE_WAIT) {
            Serial.println("Connection closing...");
        }
    }
}
```

## Troubleshooting Migration Issues

### Issue 1: Compilation Errors

**Error:** `'Ethernet' was not declared in this scope`

**Solution:** Replace singleton usage with instance:
```cpp
// Wrong
Ethernet.begin(mac);

// Correct
ethernet.begin(mac);
```

**Error:** `no matching function for call to 'EthernetClient::EthernetClient()'`

**Solution:** Provide required parameters:
```cpp
// Wrong
EthernetClient client;

// Correct
EthernetClient client(&ethernet, &chip);
```

### Issue 2: Runtime Issues

**Problem:** Network operations fail silently

**Solution:** Ensure chip interface is properly initialized:
```cpp
W5500 chip(10);  // Correct CS pin
EthernetClass ethernet(&chip);
```

**Problem:** DHCP doesn't work

**Solution:** Add DHCP maintenance and check return values:
```cpp
void loop() {
    int status = ethernet.maintain();
    if (status != DHCP_CHECK_NONE) {
        Serial.print("DHCP status: ");
        Serial.println(status);
    }
}
```

### Issue 3: Performance Issues

**Problem:** Slower than expected performance

**Solution:** Ensure you're using the correct chip interface (W5500 vs W5100) and consider SPI clock speed settings.

### Issue 4: Memory Issues

**Problem:** Running out of memory

**Solution:** The Ethernet3 library is designed to be more memory-efficient, but ensure you're not creating unnecessary instances and clean up connections properly.

## Migration Checklist

1. **Update includes** - Change to Ethernet3 headers
2. **Add chip interface** - Create W5500 or W5100 instance  
3. **Create Ethernet instance** - Replace singleton with instance
4. **Update all networking objects** - Pass ethernet and chip references
5. **Add DHCP maintenance** - Call `ethernet.maintain()` in loop
6. **Test thoroughly** - Verify all functionality works as expected
7. **Consider new features** - Multicast, enhanced status monitoring, etc.
8. **Update error handling** - Use enhanced status information

Following this guide should help you successfully migrate your Arduino Ethernet code to the enhanced Ethernet3 library while gaining access to its improved features and performance.