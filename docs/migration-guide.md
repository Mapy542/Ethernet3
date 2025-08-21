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

-   [ ] Update include statements
-   [ ] Replace singleton `Ethernet` with `EthernetClass` instance
-   [ ] Add chip interface (W5500, W5100, etc.)
-   [ ] Update client/server instantiation
-   [ ] Test basic functionality

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

## Troubleshooting Migration Issues

### Issue 1: Compilation Errors

**Error:** `'Ethernet' was not declared in this scope`

**Solution:** Replace singleton usage with instance:

```cpp
// Wrong
Ethernet.begin(mac);

// Correct
EthernetClass ethernet(&chip);

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

### Issue 3: Memory Issues

**Problem:** Running out of memory

**Solution:** The Ethernet3 library is designed to be more memory-efficient, but ensure you're not creating unnecessary instances and clean up connections properly.
