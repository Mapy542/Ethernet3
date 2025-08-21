# Basic TCP Client Example

This example demonstrates how to create a simple TCP client using the Ethernet3 library.

## Hardware Requirements
- Arduino Uno/Mega or compatible board
- W5500-based Ethernet shield or WIZ550io module
- Network connection (router/switch)

## Code

```cpp
#include <SPI.h>
#include <Ethernet3.h>
#include <EthernetClient.h>
#include <chips/w5500.h>

// Network configuration
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// Create chip interface and Ethernet instance
W5500 chip(10);  // CS pin 10
EthernetClass ethernet(&chip);
EthernetClient client(&ethernet, &chip);

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ; // Wait for serial port to connect
    }
    
    Serial.println("Ethernet3 TCP Client Example");
    
    // Initialize Ethernet with DHCP
    if (ethernet.begin(mac) == 0) {
        Serial.println("Failed to configure Ethernet using DHCP");
        // Try static IP as fallback
        IPAddress ip(192, 168, 1, 177);
        IPAddress gateway(192, 168, 1, 1);
        IPAddress subnet(255, 255, 255, 0);
        ethernet.begin(mac, ip, ethernet.dnsServerIP(), gateway, subnet);
    }
    
    // Give the Ethernet shield a second to initialize
    delay(1000);
    
    Serial.print("My IP address: ");
    Serial.println(ethernet.localIP());
    
    // Connect to web server
    Serial.println("Connecting to server...");
    if (client.connect("httpbin.org", 80)) {
        Serial.println("Connected to server");
        
        // Make HTTP request
        client.println("GET /get HTTP/1.1");
        client.println("Host: httpbin.org");
        client.println("User-Agent: Arduino/Ethernet3");
        client.println("Connection: close");
        client.println();
        
        Serial.println("Request sent");
    } else {
        Serial.println("Connection failed");
    }
}

void loop() {
    // Read and display server response
    if (client.available()) {
        char c = client.read();
        Serial.write(c);
    }
    
    // Check if connection is still active
    if (!client.connected()) {
        Serial.println();
        Serial.println("Disconnecting from server");
        client.stop();
        
        // Don't do anything more
        while (true) {
            delay(1);
            ethernet.maintain(); // Maintain DHCP lease
        }
    }
    
    // Maintain DHCP lease
    ethernet.maintain();
}
```

## Key Concepts

### 1. Chip Interface Creation
```cpp
W5500 chip(10);  // Create W5500 interface on CS pin 10
```
The Ethernet3 library uses an abstract chip interface, allowing support for different WIZnet chips.

### 2. Ethernet Instance
```cpp
EthernetClass ethernet(&chip);
```
Unlike the standard Arduino library's singleton approach, Ethernet3 uses instances that reference a specific chip.

### 3. Client Instance
```cpp
EthernetClient client(&ethernet, &chip);
```
The client requires references to both the Ethernet instance and chip interface.

### 4. DHCP with Fallback
```cpp
if (ethernet.begin(mac) == 0) {
    // DHCP failed, use static IP
    ethernet.begin(mac, ip, dns, gateway, subnet);
}
```
Always provide a fallback strategy when DHCP fails.

### 5. DHCP Maintenance
```cpp
ethernet.maintain();
```
Call this regularly to maintain DHCP lease and handle renewal/rebinding.

## Expected Output

```
Ethernet3 TCP Client Example
My IP address: 192.168.1.100
Connecting to server...
Connected to server
Request sent
HTTP/1.1 200 OK
Date: Mon, 01 Jan 2024 12:00:00 GMT
Content-Type: application/json
Content-Length: 425
Connection: close
Server: gunicorn/19.9.0
...
{
  "args": {}, 
  "headers": {
    "Accept": "*/*", 
    "Host": "httpbin.org", 
    "User-Agent": "Arduino/Ethernet3"
  }, 
  "origin": "192.168.1.100", 
  "url": "https://httpbin.org/get"
}

Disconnecting from server
```

## Troubleshooting

### Connection Issues
- Verify network cable is connected
- Check that CS pin matches your hardware (pin 10 is common)
- Ensure MAC address is unique on your network
- Verify server is accessible from your network

### DHCP Issues
- Check router DHCP settings
- Verify network supports DHCP
- Try static IP configuration for testing
- Monitor `ethernet.maintain()` return values

### DNS Issues
- Use IP addresses instead of hostnames for testing
- Verify DNS server is accessible
- Check `ethernet.dnsServerIP()` returns valid address

## Variations

### Using Static IP
```cpp
void setup() {
    // Static IP configuration
    IPAddress ip(192, 168, 1, 177);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    IPAddress dns(8, 8, 8, 8);
    
    ethernet.begin(mac, ip, dns, gateway, subnet);
}
```

### Connecting to HTTPS (limitations)
```cpp
// Note: Direct HTTPS is not supported by WIZnet chips
// Use HTTP or implement application-level encryption
if (client.connect("example.com", 443)) {
    // This will connect but won't handle TLS/SSL
    // Consider using HTTP or external TLS termination
}
```

### Error Handling
```cpp
void connectWithRetry(const char* host, uint16_t port, int maxRetries = 3) {
    for (int i = 0; i < maxRetries; i++) {
        Serial.print("Connection attempt ");
        Serial.print(i + 1);
        Serial.print(" of ");
        Serial.println(maxRetries);
        
        if (client.connect(host, port)) {
            Serial.println("Connected successfully");
            return;
        }
        
        Serial.println("Failed, retrying in 5 seconds...");
        delay(5000);
    }
    
    Serial.println("All connection attempts failed");
}
```