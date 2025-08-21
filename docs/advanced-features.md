# Advanced Features Guide

This guide covers the advanced features and capabilities of the Ethernet3 library that go beyond the standard Arduino Ethernet library functionality.

## Table of Contents

1. [Multicast UDP Communication](#multicast-udp-communication)
2. [Enhanced DHCP Management](#enhanced-dhcp-management)
3. [Abstract Chip Interface](#abstract-chip-interface)
4. [Multiple Ethernet Instances](#multiple-ethernet-instances)
5. [Advanced Socket Management](#advanced-socket-management)
6. [Performance Optimization](#performance-optimization)
7. [Error Handling and Debugging](#error-handling-and-debugging)

## Multicast UDP Communication

### Overview

Multicast allows a single sender to transmit data to multiple receivers simultaneously, making it ideal for applications like:
- Real-time sensor data distribution
- Video/audio streaming
- Network discovery protocols
- Group chat applications
- Game state synchronization

### Basic Multicast Setup

```cpp
#include <Ethernet3.h>
#include <EthernetUdp2.h>
#include <chips/w5500.h>

W5500 chip(10);
EthernetClass ethernet(&chip);
EthernetUDP udp(&ethernet, &chip);

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

void setup() {
    Serial.begin(9600);
    
    if (ethernet.begin(mac) == 0) {
        Serial.println("Failed to configure Ethernet using DHCP");
        return;
    }
    
    // Join multicast group 224.1.1.1 on port 1234
    IPAddress multicastIP(224, 1, 1, 1);
    if (udp.beginMulticast(multicastIP, 1234)) {
        Serial.println("Multicast started successfully");
        Serial.print("Listening on ");
        Serial.print(multicastIP);
        Serial.println(":1234");
    } else {
        Serial.println("Failed to start multicast");
    }
}

void loop() {
    // Check for incoming multicast packets
    int packetSize = udp.parsePacket();
    if (packetSize) {
        Serial.print("Received multicast packet from ");
        Serial.print(udp.remoteIP());
        Serial.print(":");
        Serial.print(udp.remotePort());
        Serial.print(" (");
        Serial.print(packetSize);
        Serial.println(" bytes)");
        
        // Read and display packet content
        char buffer[256];
        int len = udp.read(buffer, sizeof(buffer) - 1);
        buffer[len] = 0;
        Serial.print("Content: ");
        Serial.println(buffer);
    }
    
    ethernet.maintain();
}
```

### Managing Multiple Multicast Groups

```cpp
void setup() {
    // ... basic setup
    
    // Start with primary multicast group
    IPAddress primaryGroup(224, 1, 1, 1);
    udp.beginMulticast(primaryGroup, 1234);
    
    // Join additional groups
    IPAddress secondaryGroup(224, 2, 2, 2);
    IPAddress discoveryGroup(224, 0, 0, 251);  // mDNS group
    
    if (udp.joinMulticastGroup(secondaryGroup)) {
        Serial.println("Joined secondary group");
    }
    
    if (udp.joinMulticastGroup(discoveryGroup)) {
        Serial.println("Joined discovery group");
    }
}

void leaveGroup() {
    IPAddress groupToLeave(224, 2, 2, 2);
    if (udp.leaveMulticastGroup(groupToLeave)) {
        Serial.println("Left multicast group");
    }
}
```

### Sending Multicast Packets

```cpp
void sendMulticastData(const char* data) {
    IPAddress multicastIP(224, 1, 1, 1);
    
    if (udp.beginPacket(multicastIP, 1234)) {
        udp.write((const uint8_t*)data, strlen(data));
        if (udp.endPacket()) {
            Serial.println("Multicast packet sent successfully");
        } else {
            Serial.println("Failed to send multicast packet");
        }
    }
}

void loop() {
    static unsigned long lastSend = 0;
    
    // Send heartbeat every 5 seconds
    if (millis() - lastSend > 5000) {
        char message[64];
        sprintf(message, "Heartbeat from %s", ethernet.localIP().toString().c_str());
        sendMulticastData(message);
        lastSend = millis();
    }
    
    // Handle incoming packets...
}
```

### Multicast Address Validation

```cpp
bool validateMulticastAddress(IPAddress ip) {
    if (udp.isMulticastGroup(ip)) {
        Serial.print(ip);
        Serial.println(" is a valid multicast address");
        return true;
    } else {
        Serial.print(ip);
        Serial.println(" is NOT a multicast address");
        return false;
    }
}

void setup() {
    // ... basic setup
    
    // Test various addresses
    validateMulticastAddress(IPAddress(224, 1, 1, 1));    // Valid
    validateMulticastAddress(IPAddress(192, 168, 1, 1));  // Invalid
    validateMulticastAddress(IPAddress(239, 255, 255, 255)); // Valid (local scope)
}
```

## Enhanced DHCP Management

### Comprehensive DHCP Status Monitoring

```cpp
void monitorDHCP() {
    int status = ethernet.maintain();
    static int lastStatus = DHCP_CHECK_NONE;
    
    if (status != lastStatus) {
        switch (status) {
            case DHCP_CHECK_NONE:
                Serial.println("DHCP: No action needed");
                break;
                
            case DHCP_CHECK_RENEW_OK:
                Serial.println("DHCP: Lease renewed successfully");
                printNetworkInfo();
                break;
                
            case DHCP_CHECK_REBIND_OK:
                Serial.println("DHCP: Lease rebound successfully");
                printNetworkInfo();
                break;
                
            case DHCP_CHECK_RENEW_FAIL:
                Serial.println("DHCP: Lease renewal FAILED");
                handleDHCPFailure();
                break;
                
            case DHCP_CHECK_REBIND_FAIL:
                Serial.println("DHCP: Lease rebind FAILED");
                handleDHCPFailure();
                break;
                
            default:
                Serial.print("DHCP: Unknown status ");
                Serial.println(status);
                break;
        }
        lastStatus = status;
    }
}

void printNetworkInfo() {
    Serial.print("IP: ");
    Serial.println(ethernet.localIP());
    Serial.print("Subnet: ");
    Serial.println(ethernet.subnetMask());
    Serial.print("Gateway: ");
    Serial.println(ethernet.gatewayIP());
    Serial.print("DNS: ");
    Serial.println(ethernet.dnsServerIP());
    
    // Enhanced info from DHCP
    char* domain = ethernet.dnsDomainName();
    if (domain) {
        Serial.print("Domain: ");
        Serial.println(domain);
    }
    
    char* hostname = ethernet.hostName();
    if (hostname) {
        Serial.print("Hostname: ");
        Serial.println(hostname);
    }
}

void handleDHCPFailure() {
    Serial.println("Attempting to restart DHCP...");
    
    // Could implement fallback to static IP
    IPAddress fallbackIP(192, 168, 1, 100);
    IPAddress fallbackGateway(192, 168, 1, 1);
    IPAddress fallbackDNS(8, 8, 8, 8);
    IPAddress fallbackSubnet(255, 255, 255, 0);
    
    byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
    ethernet.begin(mac, fallbackIP, fallbackDNS, fallbackGateway, fallbackSubnet);
    
    Serial.println("Switched to static IP configuration");
    printNetworkInfo();
}
```

### DHCP Lease Timing

```cpp
class DHCPMonitor {
private:
    unsigned long lastLeaseCheck;
    unsigned long leaseCheckInterval;
    bool dhcpActive;
    
public:
    DHCPMonitor() : lastLeaseCheck(0), leaseCheckInterval(30000), dhcpActive(false) {}
    
    void begin() {
        dhcpActive = true;
        lastLeaseCheck = millis();
    }
    
    void update() {
        if (!dhcpActive) return;
        
        if (millis() - lastLeaseCheck >= leaseCheckInterval) {
            int status = ethernet.maintain();
            
            if (status != DHCP_CHECK_NONE) {
                Serial.print("DHCP maintenance: ");
                Serial.println(status);
                
                // Adjust check interval based on lease status
                if (status == DHCP_CHECK_RENEW_FAIL || status == DHCP_CHECK_REBIND_FAIL) {
                    leaseCheckInterval = 10000;  // Check more frequently on failure
                } else {
                    leaseCheckInterval = 30000;  // Normal interval
                }
            }
            
            lastLeaseCheck = millis();
        }
    }
    
    void stop() {
        dhcpActive = false;
    }
};

DHCPMonitor dhcpMonitor;

void setup() {
    // ... ethernet initialization with DHCP
    dhcpMonitor.begin();
}

void loop() {
    dhcpMonitor.update();
    // ... other code
}
```

## Abstract Chip Interface

### Supporting Multiple Chip Types

```cpp
#include <chips/w5500.h>
#include <chips/w5100.h>

// Factory function to create appropriate chip interface
EthernetChip* createChipInterface(int chipType, uint8_t csPin) {
    switch (chipType) {
        case 5500:
            return new W5500(csPin);
        case 5100:
            return new W5100(csPin);
        default:
            Serial.println("Unsupported chip type");
            return nullptr;
    }
}

void setup() {
    Serial.begin(9600);
    
    // Auto-detect or configure chip type
    int chipType = 5500;  // Could be auto-detected
    uint8_t csPin = 10;
    
    EthernetChip* chip = createChipInterface(chipType, csPin);
    if (!chip) {
        Serial.println("Failed to create chip interface");
        return;
    }
    
    EthernetClass ethernet(chip);
    
    // Test chip initialization
    if (!chip->init()) {
        Serial.println("Failed to initialize chip");
        return;
    }
    
    Serial.print("Chip type: ");
    Serial.println(chip->getChipType());
    Serial.print("CS Pin: ");
    Serial.println(chip->getCSPin());
    
    // Continue with normal setup...
}
```

### Custom Chip Interface Implementation

```cpp
class CustomEthernetChip : public EthernetChip {
private:
    // Custom implementation details
    
public:
    CustomEthernetChip(uint8_t cs_pin) : EthernetChip(cs_pin) {}
    
    bool init() override {
        // Custom initialization logic
        initSS();
        // ... custom initialization
        return true;
    }
    
    bool linkActive() override {
        // Custom link detection
        return true;  // Implement actual link detection
    }
    
    uint8_t getChipType() override {
        return 0xFF;  // Custom chip type identifier
    }
    
    void swReset() override {
        // Custom software reset implementation
    }
    
    // Implement all other required virtual methods...
};
```

## Multiple Ethernet Instances

### Dual Network Interface Example

```cpp
#include <chips/w5500.h>

// Two separate network interfaces
W5500 chip1(10);   // First interface on CS pin 10
W5500 chip2(9);    // Second interface on CS pin 9

EthernetClass ethernet1(&chip1);
EthernetClass ethernet2(&chip2);

EthernetClient client1(&ethernet1, &chip1);
EthernetServer server2(&ethernet2, &chip2, 80);

byte mac1[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte mac2[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE};

void setup() {
    Serial.begin(9600);
    
    // Initialize first interface (DHCP)
    if (ethernet1.begin(mac1)) {
        Serial.print("Interface 1 IP: ");
        Serial.println(ethernet1.localIP());
    }
    
    // Initialize second interface (static IP)
    IPAddress ip2(192, 168, 2, 100);
    ethernet2.begin(mac2, ip2);
    Serial.print("Interface 2 IP: ");
    Serial.println(ethernet2.localIP());
    
    // Start server on second interface
    server2.begin();
    
    Serial.println("Dual interface setup complete");
}

void loop() {
    // Maintain both interfaces
    ethernet1.maintain();
    // ethernet2 uses static IP, no maintenance needed
    
    // Handle client connections on interface 1
    // (outbound connections)
    
    // Handle server connections on interface 2
    EthernetClient client = server2.available();
    if (client) {
        Serial.println("Client connected to interface 2");
        // Handle client...
        client.stop();
    }
}
```

### Interface Abstraction Class

```cpp
class NetworkInterface {
private:
    EthernetChip* chip;
    EthernetClass* ethernet;
    uint8_t interfaceId;
    
public:
    NetworkInterface(uint8_t id, uint8_t csPin) : interfaceId(id) {
        chip = new W5500(csPin);
        ethernet = new EthernetClass(chip);
    }
    
    ~NetworkInterface() {
        delete ethernet;
        delete chip;
    }
    
    bool begin(byte* mac, IPAddress ip = IPAddress(0,0,0,0)) {
        if (ip == IPAddress(0,0,0,0)) {
            return ethernet->begin(mac) == 1;
        } else {
            ethernet->begin(mac, ip);
            return true;
        }
    }
    
    void maintain() {
        ethernet->maintain();
    }
    
    IPAddress localIP() {
        return ethernet->localIP();
    }
    
    EthernetClass* getEthernet() { return ethernet; }
    EthernetChip* getChip() { return chip; }
    uint8_t getId() { return interfaceId; }
};

// Usage
NetworkInterface interface1(1, 10);
NetworkInterface interface2(2, 9);

void setup() {
    byte mac1[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
    byte mac2[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE};
    
    interface1.begin(mac1);  // DHCP
    interface2.begin(mac2, IPAddress(192, 168, 2, 100));  // Static
}
```

## Advanced Socket Management

### Socket State Monitoring

```cpp
void printSocketStatus() {
    Serial.println("=== Socket Status ===");
    
    for (int i = 0; i < MAX_SOCK_NUM; i++) {
        EthernetClient client(&ethernet, &chip, i);
        uint8_t status = client.status();
        
        Serial.print("Socket ");
        Serial.print(i);
        Serial.print(": ");
        
        switch (status) {
            case SnSR::CLOSED:
                Serial.println("CLOSED");
                break;
            case SnSR::INIT:
                Serial.println("INIT");
                break;
            case SnSR::LISTEN:
                Serial.println("LISTEN");
                break;
            case SnSR::ESTABLISHED:
                Serial.print("ESTABLISHED");
                if (client.available()) {
                    Serial.print(" (");
                    Serial.print(client.available());
                    Serial.print(" bytes available)");
                }
                Serial.println();
                break;
            case SnSR::CLOSE_WAIT:
                Serial.println("CLOSE_WAIT");
                break;
            case SnSR::FIN_WAIT:
                Serial.println("FIN_WAIT");
                break;
            default:
                Serial.print("UNKNOWN (");
                Serial.print(status);
                Serial.println(")");
                break;
        }
    }
    Serial.println("====================");
}

void loop() {
    static unsigned long lastStatusPrint = 0;
    
    if (millis() - lastStatusPrint > 10000) {  // Every 10 seconds
        printSocketStatus();
        lastStatusPrint = millis();
    }
}
```

### Connection Pool Management

```cpp
class ConnectionPool {
private:
    struct Connection {
        EthernetClient* client;
        bool inUse;
        unsigned long lastActivity;
        char host[64];
        uint16_t port;
    };
    
    Connection connections[4];  // Pool of 4 connections
    EthernetClass* ethernet;
    EthernetChip* chip;
    
public:
    ConnectionPool(EthernetClass* eth, EthernetChip* ch) : ethernet(eth), chip(ch) {
        for (int i = 0; i < 4; i++) {
            connections[i].client = new EthernetClient(ethernet, chip);
            connections[i].inUse = false;
            connections[i].lastActivity = 0;
            memset(connections[i].host, 0, sizeof(connections[i].host));
            connections[i].port = 0;
        }
    }
    
    EthernetClient* getConnection(const char* host, uint16_t port) {
        // First, try to find existing connection to same host:port
        for (int i = 0; i < 4; i++) {
            if (connections[i].inUse && 
                strcmp(connections[i].host, host) == 0 && 
                connections[i].port == port &&
                connections[i].client->connected()) {
                connections[i].lastActivity = millis();
                return connections[i].client;
            }
        }
        
        // Find available connection
        for (int i = 0; i < 4; i++) {
            if (!connections[i].inUse || !connections[i].client->connected()) {
                if (connections[i].client->connect(host, port)) {
                    connections[i].inUse = true;
                    connections[i].lastActivity = millis();
                    strncpy(connections[i].host, host, sizeof(connections[i].host) - 1);
                    connections[i].port = port;
                    return connections[i].client;
                }
            }
        }
        
        return nullptr;  // No available connections
    }
    
    void releaseConnection(EthernetClient* client) {
        for (int i = 0; i < 4; i++) {
            if (connections[i].client == client) {
                connections[i].inUse = false;
                break;
            }
        }
    }
    
    void cleanup() {
        unsigned long now = millis();
        for (int i = 0; i < 4; i++) {
            if (connections[i].inUse && 
                (now - connections[i].lastActivity > 30000 || // 30 second timeout
                 !connections[i].client->connected())) {
                connections[i].client->stop();
                connections[i].inUse = false;
            }
        }
    }
};
```

## Performance Optimization

### SPI Optimization

```cpp
// Custom SPI settings for better performance
void optimizeSPI() {
    // Increase SPI clock speed if your system supports it
    SPI.beginTransaction(SPISettings(25000000, MSBFIRST, SPI_MODE0));
    
    // Note: Maximum speed depends on:
    // - Arduino board capabilities
    // - WIZnet chip specifications
    // - PCB layout and trace lengths
    // - Power supply stability
    
    // W5500 typically supports up to 33.3 MHz
    // W5100 typically supports up to 14 MHz
}
```

### Buffer Management

```cpp
class BufferedClient {
private:
    EthernetClient* client;
    uint8_t writeBuffer[1024];
    uint16_t writePos;
    
public:
    BufferedClient(EthernetClient* c) : client(c), writePos(0) {}
    
    size_t write(uint8_t data) {
        if (writePos < sizeof(writeBuffer)) {
            writeBuffer[writePos++] = data;
            return 1;
        }
        return 0;
    }
    
    size_t write(const uint8_t* data, size_t len) {
        size_t written = 0;
        while (written < len && writePos < sizeof(writeBuffer)) {
            writeBuffer[writePos++] = data[written++];
        }
        return written;
    }
    
    void flush() {
        if (writePos > 0) {
            client->write(writeBuffer, writePos);
            client->flush();
            writePos = 0;
        }
    }
    
    bool autoFlush() {
        if (writePos >= sizeof(writeBuffer) * 0.8) {  // 80% full
            flush();
            return true;
        }
        return false;
    }
};
```

### Asynchronous Operations

```cpp
class AsyncHTTPClient {
private:
    enum State {
        IDLE,
        CONNECTING,
        SENDING_REQUEST,
        READING_RESPONSE,
        COMPLETE,
        ERROR
    };
    
    EthernetClient* client;
    State state;
    String response;
    unsigned long stateTimeout;
    
public:
    AsyncHTTPClient(EthernetClient* c) : client(c), state(IDLE) {}
    
    bool beginRequest(const char* host, uint16_t port, const char* path) {
        if (state != IDLE) return false;
        
        if (client->connect(host, port)) {
            state = SENDING_REQUEST;
            stateTimeout = millis() + 5000;  // 5 second timeout
            
            client->print("GET ");
            client->print(path);
            client->println(" HTTP/1.1");
            client->print("Host: ");
            client->println(host);
            client->println("Connection: close");
            client->println();
            
            return true;
        }
        
        state = ERROR;
        return false;
    }
    
    bool update() {
        switch (state) {
            case SENDING_REQUEST:
                if (client->available() || millis() > stateTimeout) {
                    state = READING_RESPONSE;
                    stateTimeout = millis() + 10000;  // 10 second timeout
                }
                break;
                
            case READING_RESPONSE:
                while (client->available()) {
                    char c = client->read();
                    response += c;
                }
                
                if (!client->connected() || millis() > stateTimeout) {
                    state = COMPLETE;
                    return true;
                }
                break;
                
            case COMPLETE:
            case ERROR:
                return true;
                
            default:
                break;
        }
        
        return false;
    }
    
    bool isComplete() { return state == COMPLETE; }
    bool hasError() { return state == ERROR; }
    String getResponse() { return response; }
    
    void reset() {
        state = IDLE;
        response = "";
        client->stop();
    }
};
```

## Error Handling and Debugging

### Comprehensive Error Logging

```cpp
class EthernetLogger {
private:
    bool debugEnabled;
    
public:
    EthernetLogger(bool debug = true) : debugEnabled(debug) {}
    
    void logNetworkInfo(EthernetClass& ethernet) {
        if (!debugEnabled) return;
        
        Serial.println("=== Network Information ===");
        Serial.print("IP Address: ");
        Serial.println(ethernet.localIP());
        Serial.print("Subnet Mask: ");
        Serial.println(ethernet.subnetMask());
        Serial.print("Gateway: ");
        Serial.println(ethernet.gatewayIP());
        Serial.print("DNS Server: ");
        Serial.println(ethernet.dnsServerIP());
        
        char* domain = ethernet.dnsDomainName();
        if (domain) {
            Serial.print("Domain: ");
            Serial.println(domain);
        }
        
        char* hostname = ethernet.hostName();
        if (hostname) {
            Serial.print("Hostname: ");
            Serial.println(hostname);
        }
        Serial.println("===========================");
    }
    
    void logConnectionAttempt(const char* host, uint16_t port) {
        if (!debugEnabled) return;
        
        Serial.print("Attempting connection to ");
        Serial.print(host);
        Serial.print(":");
        Serial.println(port);
    }
    
    void logConnectionResult(bool success, EthernetClient& client) {
        if (!debugEnabled) return;
        
        if (success) {
            Serial.println("Connection established");
            Serial.print("Socket status: ");
            Serial.println(client.status());
        } else {
            Serial.println("Connection failed");
        }
    }
    
    void logDHCPStatus(int status) {
        if (!debugEnabled) return;
        
        Serial.print("DHCP Status: ");
        switch (status) {
            case DHCP_CHECK_NONE:
                Serial.println("No action");
                break;
            case DHCP_CHECK_RENEW_OK:
                Serial.println("Lease renewed");
                break;
            case DHCP_CHECK_REBIND_OK:
                Serial.println("Lease rebound");
                break;
            case DHCP_CHECK_RENEW_FAIL:
                Serial.println("Renewal failed");
                break;
            case DHCP_CHECK_REBIND_FAIL:
                Serial.println("Rebind failed");
                break;
            default:
                Serial.print("Unknown (");
                Serial.print(status);
                Serial.println(")");
                break;
        }
    }
    
    void setDebug(bool enabled) {
        debugEnabled = enabled;
    }
};

// Usage
EthernetLogger logger(true);

void setup() {
    // ... ethernet setup
    logger.logNetworkInfo(ethernet);
}

void loop() {
    int dhcpStatus = ethernet.maintain();
    if (dhcpStatus != DHCP_CHECK_NONE) {
        logger.logDHCPStatus(dhcpStatus);
    }
}
```

These advanced features demonstrate the enhanced capabilities of the Ethernet3 library compared to the standard Arduino Ethernet library. The modular design, enhanced error handling, and additional features like multicast support make it suitable for more complex networking applications.