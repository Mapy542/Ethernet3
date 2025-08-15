# API Reference

Complete API reference for the Ethernet3 library classes and methods.

## Ethernet3Class

Main class for managing Ethernet chip instances.

### Constructor

```cpp
Ethernet3Class(ChipType chipType = CHIP_TYPE_W5500, uint8_t csPin = 10);
```

**Parameters:**
- `chipType`: CHIP_TYPE_W5500 or CHIP_TYPE_W5100
- `csPin`: Chip select pin for SPI communication

### Network Configuration Methods

#### begin()

```cpp
int begin(uint8_t* mac);
int begin(uint8_t* mac, IPAddress ip);
int begin(uint8_t* mac, IPAddress ip, IPAddress dns);
int begin(uint8_t* mac, IPAddress ip, IPAddress dns, IPAddress gateway);
int begin(uint8_t* mac, IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet);
```

**Parameters:**
- `mac`: 6-byte MAC address array
- `ip`: Static IP address (optional, uses DHCP if not specified)
- `dns`: DNS server address
- `gateway`: Gateway address
- `subnet`: Subnet mask

**Returns:** 1 on success, 0 on failure

#### maintain()

```cpp
int maintain();
```

Maintains DHCP lease and handles renewals.

**Returns:** DHCP status code

### Network Information Methods

#### localIP()

```cpp
IPAddress localIP();
```

**Returns:** Current IP address of the interface

#### subnetMask()

```cpp
IPAddress subnetMask();
```

**Returns:** Current subnet mask

#### gatewayIP()

```cpp
IPAddress gatewayIP();
```

**Returns:** Current gateway address

#### dnsServerIP()

```cpp
IPAddress dnsServerIP();
```

**Returns:** Current DNS server address

#### linkActive()

```cpp
bool linkActive();
```

**Returns:** true if physical link is active, false otherwise

### Socket Management

#### getAvailableSocketCount()

```cpp
int getAvailableSocketCount();
```

**Returns:** Number of available sockets (W5500: 8, W5100: 4)

## EthernetClient

TCP client class for making outbound connections.

### Constructors

```cpp
EthernetClient();                           // Uses global Ethernet instance
EthernetClient(Ethernet3Class* ethernet);  // Uses specific instance
```

### Connection Methods

#### connect()

```cpp
int connect(IPAddress ip, uint16_t port);
int connect(const char* host, uint16_t port);
```

**Parameters:**
- `ip`/`host`: Target server address
- `port`: Target port number

**Returns:** 1 on success, 0 on failure

#### connected()

```cpp
uint8_t connected();
```

**Returns:** 1 if connected, 0 if disconnected

#### stop()

```cpp
void stop();
```

Closes the connection.

### Data Transfer Methods

#### available()

```cpp
int available();
```

**Returns:** Number of bytes available for reading

#### read()

```cpp
int read();
int read(uint8_t* buf, size_t size);
```

**Returns:** Single byte or number of bytes read

#### write()

```cpp
size_t write(uint8_t b);
size_t write(const uint8_t* buf, size_t size);
```

**Parameters:**
- `b`: Single byte to write
- `buf`: Buffer to write
- `size`: Number of bytes to write

**Returns:** Number of bytes written

#### print() / println()

```cpp
size_t print(const char* str);
size_t println(const char* str);
```

Print string data to the connection.

## EthernetServer

TCP server class for accepting inbound connections.

### Constructors

```cpp
EthernetServer(uint16_t port);                           // Uses global Ethernet instance
EthernetServer(uint16_t port, Ethernet3Class* ethernet); // Uses specific instance
```

### Server Methods

#### begin()

```cpp
void begin();
```

Starts listening for connections.

#### available()

```cpp
EthernetClient available();
```

**Returns:** Client object if connection pending, invalid client otherwise

#### write()

```cpp
size_t write(uint8_t b);
size_t write(const uint8_t* buf, size_t size);
```

Write data to all connected clients.

## EthernetUDP

UDP communication class with multicast support.

### Constructors

```cpp
EthernetUDP();                           // Uses global Ethernet instance
EthernetUDP(Ethernet3Class* ethernet);  // Uses specific instance
```

### Basic UDP Methods

#### begin()

```cpp
uint8_t begin(uint16_t port);
```

**Parameters:**
- `port`: Local port to bind to

**Returns:** 1 on success, 0 on failure

#### stop()

```cpp
void stop();
```

Closes the UDP socket.

### Packet Handling

#### parsePacket()

```cpp
int parsePacket();
```

**Returns:** Size of incoming packet, 0 if none

#### available()

```cpp
int available();
```

**Returns:** Number of bytes available in current packet

#### read()

```cpp
int read();
int read(unsigned char* buffer, size_t len);
int read(char* buffer, size_t len);
```

Read data from current packet.

#### remoteIP()

```cpp
IPAddress remoteIP();
```

**Returns:** IP address of packet sender

#### remotePort()

```cpp
uint16_t remotePort();
```

**Returns:** Port of packet sender

### Sending Packets

#### beginPacket()

```cpp
int beginPacket(IPAddress ip, uint16_t port);
int beginPacket(const char* host, uint16_t port);
```

Start a new outgoing packet.

#### endPacket()

```cpp
int endPacket();
```

Send the packet.

**Returns:** 1 on success, 0 on failure

#### write()

```cpp
size_t write(uint8_t byte);
size_t write(const uint8_t* buffer, size_t size);
```

Add data to outgoing packet.

### Multicast Methods

#### beginMulticast()

```cpp
uint8_t beginMulticast(IPAddress multicastIP, uint16_t port);
```

**Parameters:**
- `multicastIP`: Multicast group address (239.x.x.x recommended)
- `port`: Port to listen on

**Returns:** 1 on success, 0 on failure

**Description:** Starts UDP socket and automatically joins the multicast group.

#### joinMulticastGroup()

```cpp
bool joinMulticastGroup(IPAddress multicastIP);
```

**Parameters:**
- `multicastIP`: Multicast group to join

**Returns:** true on success, false on failure

**Description:** Joins additional multicast group (can be called multiple times).

#### leaveMulticastGroup()

```cpp
bool leaveMulticastGroup(IPAddress multicastIP);
```

**Parameters:**
- `multicastIP`: Multicast group to leave

**Returns:** true on success, false on failure

**Description:** Leaves a multicast group.

## DhcpClass

DHCP client for automatic IP configuration.

### Constructor

```cpp
DhcpClass();                           // Uses global Ethernet instance  
DhcpClass(Ethernet3Class* ethernet);  // Uses specific instance
```

### DHCP Methods

#### beginWithDHCP()

```cpp
int beginWithDHCP(uint8_t* mac, unsigned long timeout = 60000, unsigned long responseTimeout = 4000);
```

**Parameters:**
- `mac`: 6-byte MAC address
- `timeout`: Total timeout in milliseconds
- `responseTimeout`: Response timeout in milliseconds

**Returns:** 1 on success, 0 on failure

#### checkLease()

```cpp
int checkLease();
```

**Returns:** DHCP lease status

## Global Instances and Backward Compatibility

### Global Ethernet Instance

```cpp
extern Ethernet3Class Ethernet;  // Default W5500 instance on pin 10
```

### EthernetClass Typedef

```cpp
typedef Ethernet3Class EthernetClass;  // Backward compatibility
```

## Constants and Enums

### Chip Types

```cpp
typedef enum {
    CHIP_TYPE_W5100,
    CHIP_TYPE_W5500
} ChipType;
```

### DHCP Status Codes

```cpp
#define DHCP_CHECK_NONE         0
#define DHCP_CHECK_RENEW_FAIL   1
#define DHCP_CHECK_RENEW_OK     2
#define DHCP_CHECK_REBIND_FAIL  3
#define DHCP_CHECK_REBIND_OK    4
```

### Socket States

```cpp
#define SOCK_CLOSED      0x00
#define SOCK_INIT        0x13
#define SOCK_LISTEN      0x14
#define SOCK_ESTABLISHED 0x17
#define SOCK_CLOSE_WAIT  0x1C
#define SOCK_UDP         0x22
```

## Platform-Specific Features

### ESP32 Extensions

When compiled for ESP32, additional features are available:

- **16MHz SPI Speed**: Automatically configured
- **FreeRTOS Integration**: Thread-safe operations
- **Task Yielding**: Automatic yielding in long operations

### STM32 Extensions

When compiled for STM32, hardware acceleration is enabled:

- **DMA Transfers**: For high-speed SPI communication
- **Hardware Timers**: For precise timing
- **Fast GPIO**: Optimized pin operations

### Arduino Compatibility

Full compatibility maintained with standard Arduino functions:

- **Conservative Optimizations**: Stable operation on all Arduino boards
- **Yield Support**: Automatic yielding for cooperative multitasking
- **Memory Optimization**: Efficient memory usage

## Error Handling

### Common Error Codes

- **0**: Operation failed
- **1**: Operation succeeded
- **-1**: Invalid parameter
- **-2**: Resource unavailable
- **-3**: Timeout occurred

### Debugging Tips

```cpp
// Check link status before operations
if (!Ethernet.linkActive()) {
    Serial.println("Error: No link detected");
    return;
}

// Verify socket availability
if (eth.getAvailableSocketCount() == 0) {
    Serial.println("Error: No sockets available");
    return;
}

// Check connection status
if (!client.connected()) {
    Serial.println("Error: Connection lost");
    client.stop();
}
```

## Memory Usage

### RAM Usage Per Instance

- **Ethernet3Class**: ~200 bytes
- **EthernetClient**: ~50 bytes  
- **EthernetServer**: ~100 bytes
- **EthernetUDP**: ~150 bytes
- **DhcpClass**: ~300 bytes

### Socket Memory (Hardware)

- **W5500**: 32KB total (4KB per socket × 8 sockets)
- **W5100**: 16KB total (4KB per socket × 4 sockets)

## Performance Characteristics

### SPI Speed Limits

- **Arduino AVR**: Up to 8MHz
- **ESP32**: Up to 16MHz  
- **STM32**: Up to 12MHz
- **Other platforms**: Conservative speeds for compatibility

### Throughput Estimates

- **W5500**: Up to 25Mbps (limited by SPI speed)
- **W5100**: Up to 10Mbps (limited by chip capabilities)
- **Multicast**: Same as unicast (hardware offload)

## Thread Safety

### ESP32 FreeRTOS

All operations are thread-safe when using ESP32HAL:

- Critical sections protect SPI operations
- Mutex protection for socket allocation
- Task yielding prevents blocking other tasks

### Other Platforms

Not thread-safe by default. Use external synchronization if needed:

```cpp
// Example with manual synchronization
noInterrupts();
client.connect(server, port);
interrupts();
```