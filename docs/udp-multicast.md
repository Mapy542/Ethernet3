# UDP Multicast Support

Ethernet3 provides comprehensive UDP multicast support, enabling efficient group communication for applications like IoT sensor networks, real-time data distribution, and device discovery protocols.

## Overview

UDP multicast allows sending data to multiple recipients simultaneously using a single transmission. This is more efficient than unicast (one-to-one) for group communication scenarios.

### Key Features

- **Automatic Multicast MAC Calculation**: Converts multicast IP to proper MAC address
- **Hardware Configuration**: Configures W5500/W5100 registers for multicast reception  
- **Group Management**: Join and leave multicast groups dynamically
- **IGMP Considerations**: Handles router compatibility and static configuration
- **Multi-Instance Support**: Each Ethernet instance can join different groups

## Multicast Basics

### IP Address Ranges

Multicast uses specific IP address ranges:

- **224.0.0.0 to 239.255.255.255**: IPv4 multicast range
- **224.0.0.0 to 224.0.0.255**: Reserved for local network control
- **239.0.0.0 to 239.255.255.255**: Administratively scoped (organization-local)

### MAC Address Mapping

Multicast IP addresses map to Ethernet MAC addresses:

- **Format**: `01:00:5e:XX:XX:XX`
- **Calculation**: Lower 23 bits of IP → XX:XX:XX portion
- **Example**: `239.255.0.1` → `01:00:5e:7f:00:01`

## Basic Multicast Usage

### Simple Multicast Receiver

```cpp
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);
IPAddress multicastGroup(239, 255, 0, 1);

EthernetUDP udp;

void setup() {
  Serial.begin(115200);
  
  // Initialize Ethernet
  Ethernet.begin(mac, ip);
  
  // Start multicast UDP
  if (udp.beginMulticast(multicastGroup, 8080)) {
    Serial.println("Multicast UDP started");
    Serial.print("Listening on group: ");
    Serial.println(multicastGroup);
  } else {
    Serial.println("Failed to start multicast");
  }
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    Serial.print("Received multicast packet (");
    Serial.print(packetSize);
    Serial.print(" bytes) from ");
    Serial.println(udp.remoteIP());
    
    // Read packet data
    char buffer[256];
    int bytesRead = udp.read(buffer, sizeof(buffer) - 1);
    buffer[bytesRead] = '\0';
    
    Serial.print("Data: ");
    Serial.println(buffer);
  }
}
```

### Simple Multicast Sender

```cpp
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 178);
IPAddress multicastGroup(239, 255, 0, 1);

EthernetUDP udp;

void setup() {
  Serial.begin(115200);
  
  Ethernet.begin(mac, ip);
  udp.begin(8888);  // Local port for sending
  
  Serial.println("Multicast sender ready");
}

void loop() {
  // Send multicast message every 5 seconds
  String message = "Hello from ";
  message += Ethernet.localIP();
  message += " at ";
  message += millis();
  
  udp.beginPacket(multicastGroup, 8080);
  udp.print(message);
  udp.endPacket();
  
  Serial.print("Sent: ");
  Serial.println(message);
  
  delay(5000);
}
```

## Advanced Multicast Features

### Dynamic Group Management

```cpp
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);

// Multiple multicast groups
IPAddress group1(239, 255, 0, 1);  // Sensor data
IPAddress group2(239, 255, 0, 2);  // Control commands
IPAddress group3(239, 255, 0, 3);  // Status updates

EthernetUDP udp;

void setup() {
  Serial.begin(115200);
  Ethernet.begin(mac, ip);
  
  // Start multicast on primary group
  udp.beginMulticast(group1, 8080);
  
  Serial.println("Multicast manager started");
  printCommands();
}

void loop() {
  // Check for serial commands
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == "join1") {
      if (udp.joinMulticastGroup(group1)) {
        Serial.println("Joined sensor group");
      }
    } else if (command == "join2") {
      if (udp.joinMulticastGroup(group2)) {
        Serial.println("Joined control group");
      }
    } else if (command == "join3") {
      if (udp.joinMulticastGroup(group3)) {
        Serial.println("Joined status group");
      }
    } else if (command == "leave1") {
      if (udp.leaveMulticastGroup(group1)) {
        Serial.println("Left sensor group");
      }
    } else if (command == "leave2") {
      if (udp.leaveMulticastGroup(group2)) {
        Serial.println("Left control group");
      }
    } else if (command == "leave3") {
      if (udp.leaveMulticastGroup(group3)) {
        Serial.println("Left status group");
      }
    } else if (command == "help") {
      printCommands();
    }
  }
  
  // Handle incoming multicast packets
  int packetSize = udp.parsePacket();
  if (packetSize) {
    IPAddress senderIP = udp.remoteIP();
    int senderPort = udp.remotePort();
    
    char buffer[256];
    int bytesRead = udp.read(buffer, sizeof(buffer) - 1);
    buffer[bytesRead] = '\0';
    
    Serial.print("From ");
    Serial.print(senderIP);
    Serial.print(":");
    Serial.print(senderPort);
    Serial.print(" -> ");
    Serial.println(buffer);
  }
}

void printCommands() {
  Serial.println("\nCommands:");
  Serial.println("join1/join2/join3 - Join multicast group");
  Serial.println("leave1/leave2/leave3 - Leave multicast group");  
  Serial.println("help - Show this help");
}
```

### Multi-Instance Multicast

```cpp
#include <Ethernet3.h>

// Two Ethernet interfaces for different multicast networks
Ethernet3Class eth1(CHIP_TYPE_W5500, 10);  // Production network
Ethernet3Class eth2(CHIP_TYPE_W5100, 9);   // Development network

// Network configurations
byte mac1[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte mac2[] = {0xCA, 0xFE, 0xBA, 0xBE, 0xFA, 0xCE};
IPAddress ip1(192, 168, 1, 177);
IPAddress ip2(10, 0, 0, 177);

// Multicast groups
IPAddress prodGroup(239, 255, 1, 1);     // Production multicast
IPAddress devGroup(239, 255, 2, 1);      // Development multicast

EthernetUDP prodUDP(&eth1);
EthernetUDP devUDP(&eth2);

void setup() {
  Serial.begin(115200);
  
  // Initialize both networks
  eth1.begin(mac1, ip1);
  eth2.begin(mac2, ip2);
  
  // Start multicast on both interfaces
  if (prodUDP.beginMulticast(prodGroup, 8080)) {
    Serial.println("Production multicast started");
  }
  
  if (devUDP.beginMulticast(devGroup, 8080)) {
    Serial.println("Development multicast started");
  }
  
  Serial.println("Multi-instance multicast ready");
}

void loop() {
  // Handle production multicast
  int prodPacketSize = prodUDP.parsePacket();
  if (prodPacketSize) {
    Serial.print("PROD: ");
    while (prodUDP.available()) {
      Serial.write(prodUDP.read());
    }
    Serial.println();
  }
  
  // Handle development multicast
  int devPacketSize = devUDP.parsePacket();
  if (devPacketSize) {
    Serial.print("DEV: ");
    while (devUDP.available()) {
      Serial.write(devUDP.read());
    }
    Serial.println();
  }
  
  // Send status updates to both networks
  static unsigned long lastStatus = 0;
  if (millis() - lastStatus > 10000) {
    sendStatusUpdate();
    lastStatus = millis();
  }
}

void sendStatusUpdate() {
  String status = "Status: " + String(millis()) + "ms uptime";
  
  // Send to production network
  prodUDP.beginPacket(prodGroup, 8080);
  prodUDP.print("PROD " + status);
  prodUDP.endPacket();
  
  // Send to development network  
  devUDP.beginPacket(devGroup, 8080);
  devUDP.print("DEV " + status);
  devUDP.endPacket();
  
  Serial.println("Status updates sent");
}
```

## IoT Sensor Network Example

### Sensor Node (Multicast Publisher)

```cpp
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01};  // Unique per sensor
IPAddress ip(192, 168, 1, 181);
IPAddress sensorGroup(239, 255, 10, 1);  // Sensor data group

EthernetUDP udp;

// Sensor configuration
const char* sensorID = "TEMP_001";
const int sensorPin = A0;

void setup() {
  Serial.begin(115200);
  
  Ethernet.begin(mac, ip);
  udp.begin(8888);
  
  Serial.print("Sensor ");
  Serial.print(sensorID);
  Serial.println(" started");
}

void loop() {
  // Read sensor data
  int rawValue = analogRead(sensorPin);
  float temperature = (rawValue * 5.0 / 1024.0 - 0.5) * 100.0;  // TMP36
  
  // Create JSON message
  String message = "{";
  message += "\"id\":\"" + String(sensorID) + "\",";
  message += "\"type\":\"temperature\",";
  message += "\"value\":" + String(temperature, 1) + ",";
  message += "\"unit\":\"C\",";
  message += "\"timestamp\":" + String(millis()) + ",";
  message += "\"ip\":\"" + Ethernet.localIP().toString() + "\"";
  message += "}";
  
  // Send multicast message
  udp.beginPacket(sensorGroup, 8080);
  udp.print(message);
  udp.endPacket();
  
  Serial.print("Sent: ");
  Serial.println(message);
  
  delay(30000);  // Send every 30 seconds
}
```

### Data Collector (Multicast Subscriber)

```cpp
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x99};
IPAddress ip(192, 168, 1, 190);
IPAddress sensorGroup(239, 255, 10, 1);

EthernetUDP udp;

struct SensorReading {
  String id;
  String type;
  float value;
  String unit;
  unsigned long timestamp;
  IPAddress sourceIP;
};

void setup() {
  Serial.begin(115200);
  
  Ethernet.begin(mac, ip);
  
  if (udp.beginMulticast(sensorGroup, 8080)) {
    Serial.println("Data collector started");
    Serial.println("Listening for sensor data...");
  } else {
    Serial.println("Failed to start multicast");
  }
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    // Read packet
    String packet = "";
    while (udp.available()) {
      packet += (char)udp.read();
    }
    
    // Parse sensor data
    SensorReading reading;
    if (parseSensorData(packet, reading)) {
      processSensorReading(reading);
    }
  }
}

bool parseSensorData(const String& json, SensorReading& reading) {
  // Simple JSON parsing (in production, use a proper JSON library)
  int idStart = json.indexOf("\"id\":\"") + 6;
  int idEnd = json.indexOf("\"", idStart);
  if (idStart < 6 || idEnd < 0) return false;
  reading.id = json.substring(idStart, idEnd);
  
  int typeStart = json.indexOf("\"type\":\"") + 8;
  int typeEnd = json.indexOf("\"", typeStart);
  if (typeStart < 8 || typeEnd < 0) return false;
  reading.type = json.substring(typeStart, typeEnd);
  
  int valueStart = json.indexOf("\"value\":") + 8;
  int valueEnd = json.indexOf(",", valueStart);
  if (valueStart < 8 || valueEnd < 0) return false;
  reading.value = json.substring(valueStart, valueEnd).toFloat();
  
  int unitStart = json.indexOf("\"unit\":\"") + 8;
  int unitEnd = json.indexOf("\"", unitStart);
  if (unitStart < 8 || unitEnd < 0) return false;
  reading.unit = json.substring(unitStart, unitEnd);
  
  reading.sourceIP = udp.remoteIP();
  reading.timestamp = millis();
  
  return true;
}

void processSensorReading(const SensorReading& reading) {
  Serial.print("Sensor: ");
  Serial.print(reading.id);
  Serial.print(" | Type: ");
  Serial.print(reading.type);
  Serial.print(" | Value: ");
  Serial.print(reading.value);
  Serial.print(" ");
  Serial.print(reading.unit);
  Serial.print(" | From: ");
  Serial.println(reading.sourceIP);
  
  // Store in database, trigger alerts, etc.
  if (reading.type == "temperature" && reading.value > 30.0) {
    Serial.println("*** HIGH TEMPERATURE ALERT ***");
  }
}
```

## Device Discovery Protocol

### Service Announcer

```cpp
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x10};
IPAddress ip(192, 168, 1, 195);
IPAddress discoveryGroup(239, 255, 255, 1);  // Discovery group

EthernetUDP discoveryUDP;
EthernetServer webServer(80);

// Service information
const char* serviceName = "IoT Gateway";
const char* serviceType = "gateway";
const int servicePort = 80;

void setup() {
  Serial.begin(115200);
  
  Ethernet.begin(mac, ip);
  discoveryUDP.begin(8888);
  webServer.begin();
  
  Serial.println("Service announcer started");
  announceService();
}

void loop() {
  // Handle discovery requests
  int packetSize = discoveryUDP.parsePacket();
  if (packetSize) {
    String request = "";
    while (discoveryUDP.available()) {
      request += (char)discoveryUDP.read();
    }
    
    if (request == "DISCOVER") {
      handleDiscoveryRequest();
    }
  }
  
  // Handle web server clients
  EthernetClient client = webServer.available();
  if (client) {
    handleWebClient(client);
  }
  
  // Periodic service announcement
  static unsigned long lastAnnounce = 0;
  if (millis() - lastAnnounce > 60000) {  // Every minute
    announceService();
    lastAnnounce = millis();
  }
}

void announceService() {
  String announcement = "SERVICE:";
  announcement += serviceName;
  announcement += ":";
  announcement += serviceType;
  announcement += ":";
  announcement += Ethernet.localIP().toString();
  announcement += ":";
  announcement += String(servicePort);
  
  discoveryUDP.beginPacket(discoveryGroup, 8080);
  discoveryUDP.print(announcement);
  discoveryUDP.endPacket();
  
  Serial.println("Service announced: " + announcement);
}

void handleDiscoveryRequest() {
  IPAddress requesterIP = discoveryUDP.remoteIP();
  int requesterPort = discoveryUDP.remotePort();
  
  String response = "RESPONSE:";
  response += serviceName;
  response += ":";
  response += serviceType;
  response += ":";
  response += Ethernet.localIP().toString();
  response += ":";
  response += String(servicePort);
  
  discoveryUDP.beginPacket(requesterIP, requesterPort);
  discoveryUDP.print(response);
  discoveryUDP.endPacket();
  
  Serial.print("Discovery response sent to ");
  Serial.println(requesterIP);
}

void handleWebClient(EthernetClient& client) {
  // Simple web interface
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.println("<html><body>");
  client.println("<h1>" + String(serviceName) + "</h1>");
  client.println("<p>Service Type: " + String(serviceType) + "</p>");
  client.println("<p>IP Address: " + Ethernet.localIP().toString() + "</p>");
  client.println("</body></html>");
  
  delay(1);
  client.stop();
}
```

### Service Discovery Client

```cpp
#include <Ethernet3.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x20};
IPAddress ip(192, 168, 1, 200);
IPAddress discoveryGroup(239, 255, 255, 1);

EthernetUDP discoveryUDP;

struct ServiceInfo {
  String name;
  String type;
  IPAddress address;
  int port;
};

ServiceInfo discoveredServices[10];
int serviceCount = 0;

void setup() {
  Serial.begin(115200);
  
  Ethernet.begin(mac, ip);
  
  if (discoveryUDP.beginMulticast(discoveryGroup, 8080)) {
    Serial.println("Service discovery client started");
    discoverServices();
  }
}

void loop() {
  // Listen for service announcements
  int packetSize = discoveryUDP.parsePacket();
  if (packetSize) {
    String message = "";
    while (discoveryUDP.available()) {
      message += (char)discoveryUDP.read();
    }
    
    if (message.startsWith("SERVICE:") || message.startsWith("RESPONSE:")) {
      parseServiceInfo(message);
    }
  }
  
  // Check for serial commands
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == "discover") {
      discoverServices();
    } else if (command == "list") {
      listServices();
    } else if (command.startsWith("connect ")) {
      int serviceIndex = command.substring(8).toInt();
      connectToService(serviceIndex);
    }
  }
}

void discoverServices() {
  Serial.println("Broadcasting discovery request...");
  
  discoveryUDP.beginPacket(discoveryGroup, 8888);
  discoveryUDP.print("DISCOVER");
  discoveryUDP.endPacket();
}

void parseServiceInfo(const String& message) {
  // Parse: SERVICE:name:type:ip:port or RESPONSE:name:type:ip:port
  int startIndex = message.indexOf(':') + 1;
  
  int nameEnd = message.indexOf(':', startIndex);
  if (nameEnd < 0) return;
  String name = message.substring(startIndex, nameEnd);
  
  int typeEnd = message.indexOf(':', nameEnd + 1);
  if (typeEnd < 0) return;
  String type = message.substring(nameEnd + 1, typeEnd);
  
  int ipEnd = message.indexOf(':', typeEnd + 1);
  if (ipEnd < 0) return;
  String ipStr = message.substring(typeEnd + 1, ipEnd);
  
  String portStr = message.substring(ipEnd + 1);
  int port = portStr.toInt();
  
  // Check if service already known
  IPAddress serviceIP;
  serviceIP.fromString(ipStr);
  
  for (int i = 0; i < serviceCount; i++) {
    if (discoveredServices[i].address == serviceIP && 
        discoveredServices[i].port == port) {
      return;  // Already known
    }
  }
  
  // Add new service
  if (serviceCount < 10) {
    discoveredServices[serviceCount].name = name;
    discoveredServices[serviceCount].type = type;
    discoveredServices[serviceCount].address = serviceIP;
    discoveredServices[serviceCount].port = port;
    serviceCount++;
    
    Serial.print("Discovered service: ");
    Serial.print(name);
    Serial.print(" (");
    Serial.print(type);
    Serial.print(") at ");
    Serial.print(serviceIP);
    Serial.print(":");
    Serial.println(port);
  }
}

void listServices() {
  Serial.println("\nDiscovered Services:");
  for (int i = 0; i < serviceCount; i++) {
    Serial.print(i);
    Serial.print(": ");
    Serial.print(discoveredServices[i].name);
    Serial.print(" (");
    Serial.print(discoveredServices[i].type);
    Serial.print(") - ");
    Serial.print(discoveredServices[i].address);
    Serial.print(":");
    Serial.println(discoveredServices[i].port);
  }
  Serial.println("\nCommands: discover, list, connect <index>");
}

void connectToService(int index) {
  if (index < 0 || index >= serviceCount) {
    Serial.println("Invalid service index");
    return;
  }
  
  ServiceInfo& service = discoveredServices[index];
  
  Serial.print("Connecting to ");
  Serial.print(service.name);
  Serial.print(" at ");
  Serial.print(service.address);
  Serial.print(":");
  Serial.println(service.port);
  
  EthernetClient client;
  if (client.connect(service.address, service.port)) {
    Serial.println("Connected! Sending HTTP request...");
    
    client.println("GET / HTTP/1.1");
    client.print("Host: ");
    client.println(service.address);
    client.println("Connection: close");
    client.println();
    
    // Read response
    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(line);
      }
    }
    
    client.stop();
    Serial.println("Connection closed");
  } else {
    Serial.println("Connection failed");
  }
}
```

## Hardware Configuration Details

### W5500 Multicast Setup

The library automatically configures W5500 registers for multicast:

- **Socket Mode Register (Sn_MR)**: Sets UDP mode with multicast flag (0x80)
- **Interrupt Mask Register (IMR)**: Enables multicast packet reception
- **Hardware Address Filter**: Configures MAC address filtering

### W5100 Multicast Support

W5100 has limited multicast support but can receive multicast packets:

- **Promiscuous Mode**: Accepts all packets for software filtering
- **Software Filtering**: Application-level multicast group management
- **Performance Impact**: Higher CPU usage due to software filtering

## IGMP and Router Configuration

### IGMP Protocol

- **W5500/W5100**: No native IGMP support
- **Router Dependency**: Requires IGMP snooping or static configuration
- **Workaround**: Static multicast forwarding rules

### Router Configuration

Most routers require configuration for multicast forwarding:

```bash
# Example router commands (varies by manufacturer)
# Enable IGMP snooping
configure
set protocols igmp interface eth0 version 2
set protocols igmp interface eth0 query-interval 60

# Static multicast routes
ip route 239.255.0.0/16 eth0
```

### Network Switch Configuration

For managed switches, enable IGMP snooping:

- **IGMP Snooping**: Forwards multicast only to interested ports
- **Static Groups**: Manually configure multicast forwarding
- **Query Forwarding**: Forward IGMP queries between segments

## Troubleshooting

### Common Issues

**No Multicast Packets Received**:
- Check router IGMP snooping settings
- Verify firewall allows multicast traffic
- Ensure all devices on same subnet
- Test with broadcast first (255.255.255.255)

**Multicast MAC Issues**:
- Library handles MAC calculation automatically
- W5500: Hardware filtering configured
- W5100: Software filtering in place

**Performance Problems**:
- W5100: Higher CPU usage due to software filtering
- Network congestion: Limit multicast traffic
- Router overload: Use smaller multicast groups

### Debug Tips

```cpp
// Enable multicast debugging
void debugMulticast() {
  Serial.println("Multicast Debug Info:");
  Serial.print("Group IP: ");
  Serial.println(multicastGroup);
  
  // Calculate multicast MAC
  byte multicastMAC[6];
  multicastMAC[0] = 0x01;
  multicastMAC[1] = 0x00;
  multicastMAC[2] = 0x5e;
  multicastMAC[3] = multicastGroup[1] & 0x7F;
  multicastMAC[4] = multicastGroup[2];
  multicastMAC[5] = multicastGroup[3];
  
  Serial.print("Multicast MAC: ");
  for (int i = 0; i < 6; i++) {
    if (multicastMAC[i] < 0x10) Serial.print("0");
    Serial.print(multicastMAC[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
}
```

## Best Practices

1. **Use Administratively Scoped Addresses**: 239.x.x.x range for private networks
2. **Limit Group Size**: Smaller groups reduce network load
3. **Router Configuration**: Ensure IGMP snooping or static routing
4. **Error Handling**: Check return values from multicast functions
5. **Network Testing**: Test multicast before deployment
6. **Documentation**: Document multicast groups used in your application

## Next Steps

- Learn about [Platform Support](platform-support.md) for optimization options
- Check [Examples](examples.md) for complete multicast applications
- Review [API Reference](api-reference.md) for detailed method documentation
- Explore [Troubleshooting](troubleshooting.md) for common issues