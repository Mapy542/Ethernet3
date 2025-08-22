# Migration Guide: From Arduino Ethernet to Ethernet3

This guide helps you migrate existing Arduino Ethernet library code to the enhanced Ethernet3 library.

## Table of Contents

1. [Quick Migration Checklist](#quick-migration-checklist)
2. [Migration Options](#migration-options)
3. [Option 1: Backwards Compatibility Mode (Easiest)](#option-1-backwards-compatibility-mode-easiest)
4. [Option 2: Modern Dependency Injection (Recommended)](#option-2-modern-dependency-injection-recommended)
5. [Class and Method Changes](#class-and-method-changes)
6. [Common Migration Patterns](#common-migration-patterns)
7. [New Features to Consider](#new-features-to-consider)
8. [Troubleshooting Migration Issues](#troubleshooting-migration-issues)

## Quick Migration Checklist

**Choose your migration approach:**

**Option 1: Backwards Compatibility Mode (Easiest)**
-   [ ] Add `#define ETHERNET_BACKWARDS_COMPATIBILITY` before includes
-   [ ] Update include from `<Ethernet.h>` to `<Ethernet3.h>`
-   [ ] Test basic functionality

**Option 2: Modern Dependency Injection (Recommended)**
-   [ ] Update include statements
-   [ ] Replace singleton `Ethernet` with `EthernetClass` instance
-   [ ] Add chip interface (W5500, W5100, etc.)
-   [ ] Update client/server instantiation
-   [ ] Test basic functionality

## Migration Options

Ethernet3 provides two migration paths to accommodate different needs:

### Option 1: Backwards Compatibility Mode
- **Best for**: Quick migration, existing projects, learning
- **Pros**: Minimal code changes, familiar API
- **Cons**: Uses global singletons, less flexible than modern approach

### Option 2: Modern Dependency Injection
- **Best for**: New projects, maximum flexibility, multiple chip support
- **Pros**: Better testability, multiple instances, explicit dependencies
- **Cons**: More verbose, requires understanding of dependency injection

## Option 1: Backwards Compatibility Mode (Easiest)

This mode provides global singleton instances and simplified constructors that match older Ethernet library APIs.

### Step 1: Enable Compatibility Mode

Add this define before including Ethernet3:

```cpp
#define ETHERNET_BACKWARDS_COMPATIBILITY
#include <Ethernet3.h>  // Changed from <Ethernet.h>
```

### Step 2: Use Familiar API

Your existing code should work with minimal changes:

**Before (Arduino Ethernet):**
```cpp
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
EthernetClient client;
EthernetServer server(80);

void setup() {
    if (Ethernet.begin(mac) == 0) {
        Serial.println("Failed to configure Ethernet using DHCP");
    }
    server.begin();
    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());
}
```

**After (Ethernet3 with Backwards Compatibility):**
```cpp
#define ETHERNET_BACKWARDS_COMPATIBILITY  // Add this line
#include <SPI.h>
#include <Ethernet3.h>  // Changed from <Ethernet.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
EthernetClient client;      // No changes needed!
EthernetServer server(80);  // No changes needed!

void setup() {
    if (Ethernet.begin(mac) == 0) {  // No changes needed!
        Serial.println("Failed to configure Ethernet using DHCP");
    }
    server.begin();
    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());  // No changes needed!
}
```

### Global Instances Available

When backwards compatibility is enabled, these global instances are available:
- `W5500 defaultChip` - Uses CS pin 10
- `EthernetClass Ethernet` - Global ethernet instance

### Simplified Constructors Available

- `EthernetClient()` and `EthernetClient(uint8_t sock)`
- `EthernetServer(uint16_t port)`
- `EthernetUDP()`
- `DNSClient()` and `DNSClient(unsigned long timeout)`

## Option 2: Modern Dependency Injection (Recommended)

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

## New Features to Consider

Using Ethernet3 opens up opportunities to leverage new features not available in the original Arduino Ethernet library.

### HTTP Protocol Support

Ethernet3 includes comprehensive HTTP client and server functionality built on top of the existing TCP implementation.

#### HTTP Client

Replace manual HTTP request construction with the built-in HTTP client:

**Before (manual HTTP over TCP):**

```cpp
EthernetClient client;
if (client.connect("example.com", 80)) {
    client.println("GET /api/data HTTP/1.1");
    client.println("Host: example.com");
    client.println("Connection: close");
    client.println();
    
    // Manual response parsing...
    while (client.connected()) {
        if (client.available()) {
            String line = client.readStringUntil('\r');
            // Parse headers, status code, body manually
        }
    }
    client.stop();
}
```

**After (with HTTP client):**

```cpp
#include <HTTP.h>

HTTPClient httpClient(&ethernet, &chip);
HTTPResponse response = httpClient.request("GET", "http://example.com/api/data");

if (response.getStatusCode() == 200) {
    Serial.println(response.getBody());
}
```

#### HTTP Server

Replace manual HTTP response construction with the built-in HTTP server:

**Before (manual HTTP over TCP):**

```cpp
EthernetServer server(80);

void loop() {
    EthernetClient client = server.available();
    if (client) {
        String request = client.readStringUntil('\r');
        
        // Manual request parsing and routing
        if (request.indexOf("GET / ") >= 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            client.println("<html><body><h1>Hello</h1></body></html>");
        }
        client.stop();
    }
}
```

**After (with HTTP server):**

```cpp
#include <HTTP.h>

HTTPServer httpServer(&ethernet, &chip, 80);

HTTPResponse handleRoot(const HTTPRequest& request) {
    return HTTPServer::sendHTML("<html><body><h1>Hello</h1></body></html>");
}

void setup() {
    httpServer.onGET("/", handleRoot);
    httpServer.begin();
}

void loop() {
    httpServer.handleClient();
}
```

### Enhanced Chip Support

Ethernet3 provides better support for multiple WIZnet chip types with enhanced features and improved performance.

## Troubleshooting Migration Issues

### Issue 1: Compilation Errors

**Error:** `'Ethernet' was not declared in this scope`

**Solution 1 (Backwards Compatibility):** Enable compatibility mode:
```cpp
#define ETHERNET_BACKWARDS_COMPATIBILITY
#include <Ethernet3.h>

// Now you can use:
Ethernet.begin(mac);
```

**Solution 2 (Modern):** Replace singleton usage with instance:
```cpp
// Wrong
Ethernet.begin(mac);

// Correct
EthernetClass ethernet(&chip);
ethernet.begin(mac);
```

**Error:** `no matching function for call to 'EthernetClient::EthernetClient()'`

**Solution 1 (Backwards Compatibility):** Enable compatibility mode:
```cpp
#define ETHERNET_BACKWARDS_COMPATIBILITY
#include <Ethernet3.h>

// Now you can use:
EthernetClient client;
```

**Solution 2 (Modern):** Provide required parameters:
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

### Issue 3: Memory Issues

**Problem:** Running out of memory

**Solution:** The Ethernet3 library is designed to be more memory-efficient, but ensure you're not creating unnecessary instances and clean up connections properly.

## Migrating Between Ethernet3 Modes

### From Backwards Compatibility to Modern Mode

To migrate from backwards compatibility to the modern approach:

1. Remove the `#define ETHERNET_BACKWARDS_COMPATIBILITY` line
2. Create explicit chip and ethernet instances
3. Pass these instances to all constructors

**Before (Backwards Compatibility):**
```cpp
#define ETHERNET_BACKWARDS_COMPATIBILITY
#include <Ethernet3.h>

EthernetClient client;
EthernetServer server(80);

void setup() {
    Ethernet.begin(mac);
}
```

**After (Modern):**
```cpp
#include <Ethernet3.h>
#include <chips/w5500.h>

W5500 chip(10);
EthernetClass ethernet(&chip);
EthernetClient client(&ethernet, &chip);
EthernetServer server(&ethernet, &chip, 80);

void setup() {
    ethernet.begin(mac);
}
```

### From Modern to Backwards Compatibility Mode

To migrate from modern to backwards compatibility mode:

1. Add the `#define ETHERNET_BACKWARDS_COMPATIBILITY` line
2. Remove explicit chip and ethernet instances
3. Remove parameters from constructors
4. Use global `Ethernet` instance

**Before (Modern):**
```cpp
#include <Ethernet3.h>
#include <chips/w5500.h>

W5500 chip(10);
EthernetClass ethernet(&chip);
EthernetClient client(&ethernet, &chip);

void setup() {
    ethernet.begin(mac);
}
```

**After (Backwards Compatibility):**
```cpp
#define ETHERNET_BACKWARDS_COMPATIBILITY
#include <Ethernet3.h>

EthernetClient client;

void setup() {
    Ethernet.begin(mac);
}
```
