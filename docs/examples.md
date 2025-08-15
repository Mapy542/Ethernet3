# Examples Guide

This guide walks through the examples provided with Ethernet3, demonstrating how to use all the major features of the library.

## Basic Examples

### WebClient - Simple HTTP Request

**Location:** `examples/WebClient/`

The most basic example showing how to make HTTP requests.

```cpp
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
char server[] = "www.google.com";
EthernetClient client;

void setup() {
  Ethernet.begin(mac);
  if (client.connect(server, 80)) {
    client.println("GET /search?q=arduino HTTP/1.1");
    client.println("Host: www.google.com");
    client.println("Connection: close");
    client.println();
  }
}

void loop() {
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  if (!client.connected()) {
    client.stop();
    while(1);
  }
}
```

**Key Concepts:**
- Basic Ethernet initialization
- Making HTTP requests  
- Reading responses
- Connection management

### WebServer - Simple HTTP Server

**Location:** `examples/WebServer/`

Basic web server serving HTML pages.

```cpp
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);
EthernetServer server(80);

void setup() {
  Ethernet.begin(mac, ip);
  server.begin();
}

void loop() {
  EthernetClient client = server.available();
  if (client) {
    // Read request
    String request = client.readStringUntil('\r');
    
    // Send response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<h1>Hello from Ethernet3!</h1>");
    
    delay(1);
    client.stop();
  }
}
```

**Key Concepts:**
- Server initialization
- Handling client connections
- HTTP response formatting
- Client lifecycle management

## Advanced Examples

### MultiInstanceDemo - Multiple Ethernet Chips

**Location:** `examples/MultiInstanceDemo/`

Demonstrates using W5100 and W5500 chips simultaneously.

```cpp
#include <Ethernet3.h>

// Create instances for different chips
Ethernet3Class eth1(CHIP_TYPE_W5500, 10);
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);

byte mac1[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01};
byte mac2[] = {0xCA, 0xFE, 0xBA, 0xBE, 0xFA, 0x02};

void setup() {
  // Initialize both interfaces
  eth1.begin(mac1);  // DHCP on W5500
  eth2.begin(mac2);  // DHCP on W5100
  
  Serial.print("W5500 IP: ");
  Serial.println(eth1.localIP());
  Serial.print("W5100 IP: ");
  Serial.println(eth2.localIP());
}

void loop() {
  // Monitor link status
  static bool link1 = false, link2 = false;
  
  if (eth1.linkActive() != link1) {
    link1 = eth1.linkActive();
    Serial.print("W5500 link: ");
    Serial.println(link1 ? "UP" : "DOWN");
  }
  
  if (eth2.linkActive() != link2) {
    link2 = eth2.linkActive();
    Serial.print("W5100 link: ");
    Serial.println(link2 ? "UP" : "DOWN");
  }
  
  delay(1000);
}
```

**Key Concepts:**
- Multi-instance creation
- Independent network configuration
- Link status monitoring
- Resource isolation

### Ethernet3ModernizationDemo - New Features

**Location:** `examples/Ethernet3ModernizationDemo/`

Showcases link monitoring and UDP multicast capabilities.

```cpp
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress multicastGroup(239, 255, 0, 1);
EthernetUDP udp;

void setup() {
  Ethernet.begin(mac);
  
  // Start multicast UDP
  if (udp.beginMulticast(multicastGroup, 8080)) {
    Serial.println("Multicast started");
  }
  
  // Check initial link status
  if (Ethernet.linkActive()) {
    Serial.println("Link is active");
  } else {
    Serial.println("No link detected");
  }
}

void loop() {
  // Handle multicast packets
  int packetSize = udp.parsePacket();
  if (packetSize) {
    Serial.print("Received ");
    Serial.print(packetSize);
    Serial.println(" bytes");
    
    // Echo packet back
    IPAddress sender = udp.remoteIP();
    udp.beginPacket(multicastGroup, 8080);
    udp.print("Echo from ");
    udp.print(Ethernet.localIP());
    udp.endPacket();
  }
  
  // Send periodic multicast message
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 10000) {
    udp.beginPacket(multicastGroup, 8080);
    udp.print("Hello from ");
    udp.print(Ethernet.localIP());
    udp.endPacket();
    lastSend = millis();
  }
}
```

**Key Concepts:**
- Link status detection
- Multicast group joining
- Bidirectional multicast communication
- Periodic messaging

## Multi-Instance Network Classes

### MultiInstanceClientServerDemo

**Location:** `examples/MultiInstanceClientServerDemo/`

Complete demonstration of multi-instance clients and servers.

```cpp
#include <Ethernet3.h>

Ethernet3Class eth1(CHIP_TYPE_W5500, 10);
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);

// Instance-specific network objects
EthernetServer server1(80, &eth1);
EthernetServer server2(8080, &eth2);
EthernetClient client1(&eth1);
EthernetClient client2(&eth2);

void setup() {
  eth1.begin(mac1, ip1);
  eth2.begin(mac2, ip2);
  
  server1.begin();
  server2.begin();
  
  Serial.println("Multi-instance servers started");
}

void loop() {
  // Handle server 1 clients
  EthernetClient client = server1.available();
  if (client) {
    handleWebClient(client, "Server1 (W5500)");
  }
  
  // Handle server 2 clients
  client = server2.available();
  if (client) {
    handleWebClient(client, "Server2 (W5100)");
  }
  
  // Demonstrate outbound connections
  static unsigned long lastConnect = 0;
  if (millis() - lastConnect > 30000) {
    testOutboundConnections();
    lastConnect = millis();
  }
}

void handleWebClient(EthernetClient& client, const char* serverName) {
  // Read request
  while (client.connected() && client.available()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break;
  }
  
  // Send response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.print("<h1>");
  client.print(serverName);
  client.println("</h1>");
  client.print("<p>Uptime: ");
  client.print(millis());
  client.println("ms</p>");
  
  delay(1);
  client.stop();
}

void testOutboundConnections() {
  // Test connection via W5500
  if (client1.connect("httpbin.org", 80)) {
    client1.println("GET /ip HTTP/1.1");
    client1.println("Host: httpbin.org");
    client1.println();
    
    // Read response
    while (client1.connected()) {
      if (client1.available()) {
        Serial.write(client1.read());
      }
    }
    client1.stop();
  }
}
```

**Key Concepts:**
- Multi-instance servers
- Instance-specific clients
- Resource allocation
- Independent operation

### UDPMultiInstanceDemo

**Location:** `examples/UDPMultiInstanceDemo/`

UDP communication with multiple Ethernet instances.

```cpp
#include <Ethernet3.h>

Ethernet3Class eth1(CHIP_TYPE_W5500, 10);
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);

EthernetUDP udp1(&eth1);
EthernetUDP udp2(&eth2);

void setup() {
  eth1.begin(mac1, ip1);
  eth2.begin(mac2, ip2);
  
  udp1.begin(8888);
  udp2.begin(9999);
  
  Serial.println("UDP instances started");
}

void loop() {
  // Handle UDP on first interface
  int packetSize1 = udp1.parsePacket();
  if (packetSize1) {
    Serial.print("UDP1: ");
    while (udp1.available()) {
      Serial.write(udp1.read());
    }
    Serial.println();
    
    // Echo back
    udp1.beginPacket(udp1.remoteIP(), udp1.remotePort());
    udp1.print("Echo from eth1");
    udp1.endPacket();
  }
  
  // Handle UDP on second interface
  int packetSize2 = udp2.parsePacket();
  if (packetSize2) {
    Serial.print("UDP2: ");
    while (udp2.available()) {
      Serial.write(udp2.read());
    }
    Serial.println();
    
    // Echo back
    udp2.beginPacket(udp2.remoteIP(), udp2.remotePort());
    udp2.print("Echo from eth2");
    udp2.endPacket();
  }
}
```

**Key Concepts:**
- Multi-instance UDP
- Instance-specific packet handling
- Independent echo servers
- Port separation

## Platform Optimization Examples

### PlatformOptimizationDemo

**Location:** `examples/PlatformOptimizationDemo/`

Demonstrates platform-specific optimizations.

```cpp
#include <Ethernet3.h>

void setup() {
  Serial.begin(115200);
  
  // Display platform information
  Serial.println("Platform Information:");
#ifdef ESP32
  Serial.println("ESP32 detected");
  Serial.println("Features: 16MHz SPI, FreeRTOS, dual-core");
#elif defined(STM32F4)
  Serial.println("STM32F4 detected");
  Serial.println("Features: DMA, 12MHz SPI, hardware acceleration");
#else
  Serial.println("Arduino/Generic platform");
  Serial.println("Features: Conservative optimizations");
#endif
  
  // Initialize with platform optimizations
  Ethernet.begin(mac);
  
  // Performance test
  performanceTest();
}

void performanceTest() {
  Serial.println("\nPerformance Test:");
  
  unsigned long start = micros();
  
  // Test SPI performance
  for (int i = 0; i < 1000; i++) {
    Ethernet.linkActive();  // Quick SPI operation
  }
  
  unsigned long elapsed = micros() - start;
  Serial.print("1000 link checks took: ");
  Serial.print(elapsed);
  Serial.println(" microseconds");
  
  float opsPerSecond = 1000000.0 / (elapsed / 1000.0);
  Serial.print("Operations per second: ");
  Serial.println(opsPerSecond, 0);
}

void loop() {
  // Monitor performance continuously
  static unsigned long lastTest = 0;
  if (millis() - lastTest > 10000) {
    performanceTest();
    lastTest = millis();
  }
  
  delay(100);
}
```

**Key Concepts:**
- Platform detection
- Automatic optimization selection
- Performance measurement
- Feature availability

## Specialized Examples

### DhcpAddressPrinter

**Location:** `examples/DhcpAddressPrinter/`

Demonstrates DHCP with lease management.

```cpp
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

void setup() {
  Serial.begin(115200);
  
  Serial.println("Starting DHCP...");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("DHCP failed");
    
    // Try static configuration
    IPAddress ip(192, 168, 1, 177);
    Ethernet.begin(mac, ip);
    Serial.println("Using static IP");
  } else {
    Serial.println("DHCP successful");
  }
  
  printNetworkInfo();
}

void loop() {
  // Maintain DHCP lease
  switch (Ethernet.maintain()) {
    case 1:
      Serial.println("DHCP renewed failed");
      break;
    case 2:
      Serial.println("DHCP renewed success");
      printNetworkInfo();
      break;
    case 3:
      Serial.println("DHCP rebind failed");
      break;
    case 4:
      Serial.println("DHCP rebind success");
      printNetworkInfo();
      break;
  }
  
  delay(10000);
}

void printNetworkInfo() {
  Serial.print("IP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("Subnet: ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("DNS: ");
  Serial.println(Ethernet.dnsServerIP());
}
```

**Key Concepts:**
- DHCP lease management
- Fallback to static IP
- Network information display
- Lease renewal handling

### ChatServer - Multi-Client Server

**Location:** `examples/ChatServer/`

Advanced server handling multiple simultaneous clients.

```cpp
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);
EthernetServer server(23);  // Telnet port

EthernetClient clients[8];  // Maximum clients
boolean clientConnected[8];

void setup() {
  Ethernet.begin(mac, ip);
  server.begin();
  
  for (int i = 0; i < 8; i++) {
    clientConnected[i] = false;
  }
  
  Serial.println("Chat server started");
}

void loop() {
  // Check for new clients
  EthernetClient newClient = server.available();
  if (newClient) {
    // Find available slot
    for (int i = 0; i < 8; i++) {
      if (!clientConnected[i]) {
        clients[i] = newClient;
        clientConnected[i] = true;
        
        clients[i].println("Welcome to the chat server!");
        broadcastMessage("User " + String(i) + " joined", i);
        break;
      }
    }
  }
  
  // Handle existing clients
  for (int i = 0; i < 8; i++) {
    if (clientConnected[i]) {
      if (clients[i].connected()) {
        if (clients[i].available()) {
          String message = clients[i].readStringUntil('\n');
          message.trim();
          
          if (message.length() > 0) {
            String fullMessage = "User " + String(i) + ": " + message;
            broadcastMessage(fullMessage, i);
          }
        }
      } else {
        // Client disconnected
        clients[i].stop();
        clientConnected[i] = false;
        broadcastMessage("User " + String(i) + " left", i);
      }
    }
  }
}

void broadcastMessage(String message, int sender) {
  Serial.println(message);
  
  for (int i = 0; i < 8; i++) {
    if (clientConnected[i] && i != sender) {
      clients[i].println(message);
    }
  }
}
```

**Key Concepts:**
- Multi-client management
- Message broadcasting
- Connection state tracking
- Resource cleanup

## Testing Your Setup

### Network Discovery Script

Use this script to test if your Ethernet3 setup is working:

```cpp
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

void setup() {
  Serial.begin(115200);
  Serial.println("Ethernet3 Network Discovery Test");
  
  // Test 1: Initialize Ethernet
  Serial.print("Test 1 - Initializing Ethernet... ");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("FAILED");
    Serial.println("Trying static IP...");
    
    IPAddress ip(192, 168, 1, 177);
    Ethernet.begin(mac, ip);
  }
  Serial.println("OK");
  
  // Test 2: Check link status
  Serial.print("Test 2 - Checking link status... ");
  if (Ethernet.linkActive()) {
    Serial.println("OK - Link active");
  } else {
    Serial.println("FAILED - No link detected");
  }
  
  // Test 3: Display network info
  Serial.println("Test 3 - Network Information:");
  Serial.print("  IP Address: ");
  Serial.println(Ethernet.localIP());
  Serial.print("  Subnet Mask: ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("  Gateway: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("  DNS Server: ");
  Serial.println(Ethernet.dnsServerIP());
  
  // Test 4: Ping test (HTTP request)
  Serial.print("Test 4 - HTTP connectivity test... ");
  EthernetClient client;
  if (client.connect("httpbin.org", 80)) {
    Serial.println("OK - Can connect to internet");
    client.stop();
  } else {
    Serial.println("FAILED - Cannot connect to internet");
  }
  
  Serial.println("\nSetup complete. Monitor link status...");
}

void loop() {
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

## Example Usage Patterns

### Load Balancing

```cpp
// Distribute requests across multiple interfaces
Ethernet3Class eth1(CHIP_TYPE_W5500, 10);
Ethernet3Class eth2(CHIP_TYPE_W5500, 9);

class LoadBalancer {
  int current = 0;
  Ethernet3Class* interfaces[2] = {&eth1, &eth2};
  
public:
  EthernetClient getClient() {
    EthernetClient client(interfaces[current]);
    current = (current + 1) % 2;
    return client;
  }
};
```

### Redundancy

```cpp
// Automatic failover between interfaces
bool connectReliably(const char* host, int port) {
  EthernetClient client1(&eth1);
  if (client1.connect(host, port)) {
    return true;
  }
  
  EthernetClient client2(&eth2);
  return client2.connect(host, port);
}
```

### Network Segmentation

```cpp
// Different networks for different purposes
Ethernet3Class publicNet(CHIP_TYPE_W5500, 10);   // Internet access
Ethernet3Class sensorNet(CHIP_TYPE_W5100, 9);    // Local sensors

void handlePublicTraffic() {
  EthernetClient client(&publicNet);
  // Handle internet communications
}

void handleSensorData() {
  EthernetUDP udp(&sensorNet);
  // Handle local sensor network
}
```

## Next Steps

1. **Start Simple**: Begin with WebClient or WebServer examples
2. **Add Features**: Try MultiInstanceDemo for dual-chip operation  
3. **Explore Multicast**: Use Ethernet3ModernizationDemo for group communication
4. **Optimize**: Test PlatformOptimizationDemo for your platform
5. **Build Applications**: Combine concepts for your specific use case

For complete API documentation, see [API Reference](api-reference.md).
For troubleshooting help, see [Troubleshooting Guide](troubleshooting.md).