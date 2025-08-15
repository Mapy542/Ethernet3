# Multi-Instance Architecture

The Ethernet3 library's multi-instance architecture allows you to use multiple Ethernet chips simultaneously in the same application. This is useful for applications requiring multiple network interfaces, network isolation, or redundant connections.

## Overview

Traditional Ethernet libraries use singleton patterns, limiting you to one Ethernet interface per application. Ethernet3 eliminates this restriction by:

- **Per-Instance State**: Each Ethernet3Class instance maintains its own state
- **Independent Socket Management**: Separate socket pools for each chip  
- **Resource Isolation**: No conflicts between different interfaces
- **Backward Compatibility**: Existing single-instance code works unchanged

## Architecture Diagram

```
Application Layer
├── EthernetClient(&eth1) ──┐
├── EthernetServer(&eth1) ──┼── Ethernet3Class eth1 (W5500)
├── EthernetUDP(&eth1) ─────┘
│
├── EthernetClient(&eth2) ──┐  
├── EthernetServer(&eth2) ──┼── Ethernet3Class eth2 (W5100)
└── EthernetUDP(&eth2) ─────┘
```

## Creating Multiple Instances

### Basic Multi-Instance Setup

```cpp
#include <Ethernet3.h>

// Create two Ethernet instances with different chips and pins
Ethernet3Class eth1(CHIP_TYPE_W5500, 10);  // W5500 on CS pin 10
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);   // W5100 on CS pin 9

// Network configurations
byte mac1[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte mac2[] = {0xCA, 0xFE, 0xBA, 0xBE, 0xFA, 0xCE};

IPAddress ip1(192, 168, 1, 177);    // Network 1
IPAddress ip2(10, 0, 0, 177);       // Network 2

void setup() {
  Serial.begin(115200);
  
  // Initialize both interfaces
  eth1.begin(mac1, ip1);
  eth2.begin(mac2, ip2);
  
  // Check both connections
  if (eth1.linkActive()) {
    Serial.println("Eth1 (W5500) connected");
    Serial.print("IP: ");
    Serial.println(eth1.localIP());
  }
  
  if (eth2.linkActive()) {
    Serial.println("Eth2 (W5100) connected");
    Serial.print("IP: ");  
    Serial.println(eth2.localIP());
  }
}
```

### Socket Management

Each instance manages its own socket pool:

```cpp
// W5500 provides 8 sockets (0-7)
Ethernet3Class eth1(CHIP_TYPE_W5500, 10);

// W5100 provides 4 sockets (0-3)  
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);

void setup() {
  eth1.begin(mac1, ip1);  // Has 8 sockets available
  eth2.begin(mac2, ip2);  // Has 4 sockets available
  
  // Each instance tracks its socket usage independently
  Serial.print("Eth1 available sockets: ");
  Serial.println(eth1.getAvailableSocketCount());
  
  Serial.print("Eth2 available sockets: ");
  Serial.println(eth2.getAvailableSocketCount());
}
```

## Multi-Instance Network Classes

All network classes support association with specific Ethernet instances:

### EthernetClient Multi-Instance

```cpp
#include <Ethernet3.h>

Ethernet3Class eth1(CHIP_TYPE_W5500, 10);
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);

void setup() {
  // Initialize both interfaces
  eth1.begin(mac1, ip1);
  eth2.begin(mac2, ip2);
  
  // Create clients for each interface
  EthernetClient client1(&eth1);  // Uses eth1's socket pool
  EthernetClient client2(&eth2);  // Uses eth2's socket pool
  
  // Connect to different servers via different interfaces
  if (client1.connect("server1.example.com", 80)) {
    Serial.println("Client1 connected via W5500");
    client1.println("GET / HTTP/1.1");
    client1.println("Host: server1.example.com");
    client1.println();
  }
  
  if (client2.connect("server2.example.com", 80)) {
    Serial.println("Client2 connected via W5100");  
    client2.println("GET / HTTP/1.1");
    client2.println("Host: server2.example.com");
    client2.println();
  }
}

void loop() {
  // Handle responses from both clients independently
  if (client1.available()) {
    Serial.print("Client1: ");
    Serial.write(client1.read());
  }
  
  if (client2.available()) {
    Serial.print("Client2: ");
    Serial.write(client2.read());
  }
}
```

### EthernetServer Multi-Instance

```cpp
#include <Ethernet3.h>

Ethernet3Class eth1(CHIP_TYPE_W5500, 10);
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);

// Servers on different interfaces and ports
EthernetServer server1(80, &eth1);   // Port 80 on eth1
EthernetServer server2(8080, &eth2); // Port 8080 on eth2

void setup() {
  eth1.begin(mac1, ip1);
  eth2.begin(mac2, ip2);
  
  server1.begin();
  server2.begin();
  
  Serial.println("Server1 (W5500) started on port 80");
  Serial.println("Server2 (W5100) started on port 8080");
}

void loop() {
  // Handle clients on both servers
  EthernetClient client1 = server1.available();
  if (client1) {
    handleClient(client1, "Server1");
  }
  
  EthernetClient client2 = server2.available();
  if (client2) {
    handleClient(client2, "Server2");
  }
}

void handleClient(EthernetClient& client, const char* serverName) {
  Serial.print(serverName);
  Serial.println(" - New client connected");
  
  // Read request
  while (client.connected() && client.available()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break;
  }
  
  // Send response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.print("<h1>Hello from ");
  client.print(serverName);
  client.println("!</h1>");
  
  delay(1);
  client.stop();
}
```

### EthernetUDP Multi-Instance

```cpp
#include <Ethernet3.h>

Ethernet3Class eth1(CHIP_TYPE_W5500, 10);
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);

EthernetUDP udp1(&eth1);  // UDP on eth1
EthernetUDP udp2(&eth2);  // UDP on eth2

void setup() {
  eth1.begin(mac1, ip1);
  eth2.begin(mac2, ip2);
  
  // Start UDP on different ports
  udp1.begin(8888);  // Port 8888 on eth1
  udp2.begin(9999);  // Port 9999 on eth2
  
  Serial.println("UDP services started");
}

void loop() {
  // Handle UDP packets on both interfaces
  int packetSize1 = udp1.parsePacket();
  if (packetSize1) {
    Serial.print("UDP1 received ");
    Serial.print(packetSize1);
    Serial.println(" bytes");
    
    // Echo packet back
    IPAddress remoteIP = udp1.remoteIP();
    int remotePort = udp1.remotePort();
    
    udp1.beginPacket(remoteIP, remotePort);
    udp1.write("Echo from UDP1");
    udp1.endPacket();
  }
  
  int packetSize2 = udp2.parsePacket();
  if (packetSize2) {
    Serial.print("UDP2 received ");
    Serial.print(packetSize2);
    Serial.println(" bytes");
    
    IPAddress remoteIP = udp2.remoteIP();
    int remotePort = udp2.remotePort();
    
    udp2.beginPacket(remoteIP, remotePort);
    udp2.write("Echo from UDP2");
    udp2.endPacket();
  }
}
```

### DHCP Multi-Instance

```cpp
#include <Ethernet3.h>

Ethernet3Class eth1(CHIP_TYPE_W5500, 10);
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);

void setup() {
  Serial.begin(115200);
  
  // DHCP on both interfaces
  if (eth1.begin(mac1) != 0) {
    Serial.print("Eth1 DHCP IP: ");
    Serial.println(eth1.localIP());
  } else {
    Serial.println("Eth1 DHCP failed");
  }
  
  if (eth2.begin(mac2) != 0) {
    Serial.print("Eth2 DHCP IP: ");
    Serial.println(eth2.localIP());
  } else {
    Serial.println("Eth2 DHCP failed");
  }
}

void loop() {
  // DHCP lease renewal is handled automatically per instance
  eth1.maintain();  // Maintain DHCP lease for eth1
  eth2.maintain();  // Maintain DHCP lease for eth2
  
  delay(1000);
}
```

## Advanced Multi-Instance Patterns

### Load Balancing

```cpp
#include <Ethernet3.h>

Ethernet3Class eth1(CHIP_TYPE_W5500, 10);
Ethernet3Class eth2(CHIP_TYPE_W5500, 9);

class LoadBalancer {
private:
  Ethernet3Class* interfaces[2];
  int currentInterface;
  
public:
  LoadBalancer(Ethernet3Class* eth1, Ethernet3Class* eth2) 
    : currentInterface(0) {
    interfaces[0] = eth1;
    interfaces[1] = eth2;
  }
  
  EthernetClient getClient() {
    // Round-robin between interfaces
    EthernetClient client(interfaces[currentInterface]);
    currentInterface = (currentInterface + 1) % 2;
    return client;
  }
};

LoadBalancer balancer(&eth1, &eth2);

void setup() {
  eth1.begin(mac1, ip1);
  eth2.begin(mac2, ip2);
}

void loop() {
  // Get client from load balancer
  EthernetClient client = balancer.getClient();
  
  if (client.connect("api.example.com", 80)) {
    // Make request
    client.println("GET /api/data HTTP/1.1");
    client.println("Host: api.example.com");
    client.println();
    
    // Read response
    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(line);
      }
    }
    client.stop();
  }
  
  delay(5000);
}
```

### Network Isolation

```cpp
#include <Ethernet3.h>

// Separate networks for different purposes
Ethernet3Class publicEth(CHIP_TYPE_W5500, 10);   // Public internet
Ethernet3Class privateEth(CHIP_TYPE_W5100, 9);   // Private/sensor network

// MAC addresses for different networks
byte publicMAC[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte privateMAC[] = {0xCA, 0xFE, 0xBA, 0xBE, 0xFA, 0xCE};

// Network configurations
IPAddress publicIP(192, 168, 1, 177);     // Public network
IPAddress privateIP(10, 0, 0, 177);       // Private network

void setup() {
  // Initialize both networks
  publicEth.begin(publicMAC, publicIP);
  privateEth.begin(privateMAC, privateIP);
  
  Serial.println("Network isolation setup complete");
}

void loop() {
  // Handle public internet communications
  handlePublicNetwork();
  
  // Handle private sensor network
  handlePrivateNetwork();
  
  delay(100);
}

void handlePublicNetwork() {
  static EthernetClient webClient(&publicEth);
  
  // Upload data to cloud service
  if (!webClient.connected()) {
    if (webClient.connect("api.cloudservice.com", 443)) {
      webClient.println("POST /data HTTP/1.1");
      webClient.println("Host: api.cloudservice.com");
      webClient.println("Content-Type: application/json");
      webClient.println();
      webClient.println("{\"sensor\":\"data\"}");
    }
  }
}

void handlePrivateNetwork() {
  static EthernetUDP sensorUDP(&privateEth);
  static bool udpStarted = false;
  
  if (!udpStarted) {
    sensorUDP.begin(8888);
    udpStarted = true;
  }
  
  // Listen for sensor data
  int packetSize = sensorUDP.parsePacket();
  if (packetSize) {
    // Process sensor data
    char buffer[256];
    int bytesRead = sensorUDP.read(buffer, sizeof(buffer) - 1);
    buffer[bytesRead] = '\0';
    
    Serial.print("Sensor data: ");
    Serial.println(buffer);
  }
}
```

### Redundant Connections

```cpp
#include <Ethernet3.h>

class RedundantEthernet {
private:
  Ethernet3Class* primary;
  Ethernet3Class* backup;
  bool usingBackup;
  
public:
  RedundantEthernet(Ethernet3Class* p, Ethernet3Class* b) 
    : primary(p), backup(b), usingBackup(false) {}
  
  bool isConnected() {
    if (!usingBackup && primary->linkActive()) {
      return true;
    } else if (backup->linkActive()) {
      if (!usingBackup) {
        Serial.println("Switching to backup connection");
        usingBackup = true;
      }
      return true;
    }
    return false;
  }
  
  EthernetClient getClient() {
    if (!usingBackup && primary->linkActive()) {
      return EthernetClient(primary);
    } else if (backup->linkActive()) {
      return EthernetClient(backup);
    }
    return EthernetClient();  // No connection available
  }
  
  IPAddress localIP() {
    return usingBackup ? backup->localIP() : primary->localIP();
  }
};

Ethernet3Class eth1(CHIP_TYPE_W5500, 10);  // Primary
Ethernet3Class eth2(CHIP_TYPE_W5500, 9);   // Backup

RedundantEthernet redundantEth(&eth1, &eth2);

void setup() {
  eth1.begin(mac1, ip1);
  eth2.begin(mac2, ip2);
  
  Serial.println("Redundant Ethernet initialized");
}

void loop() {
  if (redundantEth.isConnected()) {
    EthernetClient client = redundantEth.getClient();
    
    if (client.connect("server.example.com", 80)) {
      Serial.print("Connected via: ");
      Serial.println(redundantEth.localIP());
      
      client.println("GET / HTTP/1.1");
      client.println("Host: server.example.com");
      client.println();
      
      // Handle response
      while (client.connected() && client.available()) {
        Serial.write(client.read());
      }
      client.stop();
    }
  } else {
    Serial.println("No network connection available");
  }
  
  delay(10000);
}
```

## Best Practices

### Resource Management

```cpp
// Always check socket availability before creating clients
void createClientSafely(Ethernet3Class* eth) {
  if (eth->getAvailableSocketCount() > 0) {
    EthernetClient client(eth);
    // Use client...
  } else {
    Serial.println("No sockets available");
  }
}
```

### Error Handling

```cpp
// Monitor link status for each interface
void monitorInterfaces() {
  static bool eth1WasActive = false;
  static bool eth2WasActive = false;
  
  bool eth1Active = eth1.linkActive();
  bool eth2Active = eth2.linkActive();
  
  if (eth1Active != eth1WasActive) {
    Serial.print("Eth1 link ");
    Serial.println(eth1Active ? "UP" : "DOWN");
    eth1WasActive = eth1Active;
  }
  
  if (eth2Active != eth2WasActive) {
    Serial.print("Eth2 link ");
    Serial.println(eth2Active ? "UP" : "DOWN");
    eth2WasActive = eth2Active;
  }
}
```

### Memory Optimization

```cpp
// Use static instances to avoid memory fragmentation
static EthernetClient globalClient1(&eth1);
static EthernetClient globalClient2(&eth2);

void useStaticClients() {
  // Reuse the same client instances
  if (!globalClient1.connected()) {
    globalClient1.connect("server1.com", 80);
  }
  
  if (!globalClient2.connected()) {
    globalClient2.connect("server2.com", 80);
  }
}
```

## Backward Compatibility

Existing single-instance code continues to work unchanged:

```cpp
// This code works exactly as before
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

void setup() {
  Ethernet.begin(mac);  // Uses default global instance
}

void loop() {
  EthernetClient client;  // Uses default global instance
  client.connect("example.com", 80);
}
```

The global `Ethernet` instance is actually an `Ethernet3Class` instance with default settings (W5500, pin 10).

## Performance Considerations

- **Socket Limits**: W5500 = 8 sockets, W5100 = 4 sockets per instance
- **Memory Usage**: Each instance uses ~200 bytes of RAM
- **SPI Bandwidth**: Share SPI bus between instances (hardware limitation)
- **Processing Overhead**: Minimal - instances are lightweight

## Next Steps

- Learn about [UDP Multicast](udp-multicast.md) for group communication
- Explore [Platform Support](platform-support.md) for optimization options
- Check [Examples](examples.md) for complete multi-instance applications
- Review [API Reference](api-reference.md) for detailed method documentation