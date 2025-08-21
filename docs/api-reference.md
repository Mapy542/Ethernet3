# Ethernet3 API Reference

## Core Classes

### EthernetClass

The main Ethernet interface class that manages network configuration and chip communication.

#### Constructor

```cpp
EthernetClass(EthernetChip* chip)
```

Creates an Ethernet instance using the specified chip interface.

#### Initialization Methods

##### DHCP Initialization

```cpp
int begin(uint8_t* mac_address)
```

Initialize with MAC address using DHCP. Returns 1 if successful, 0 if DHCP failed.

##### Static IP Initialization

```cpp
void begin(uint8_t* mac_address, IPAddress local_ip)
void begin(uint8_t* mac_address, IPAddress local_ip, IPAddress dns_server)
void begin(uint8_t* mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway)
void begin(uint8_t* mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet)
```

Initialize with static network configuration. Auto-configures missing parameters.

##### WIZ550io Initialization (when WIZ550io_WITH_MACADDRESS is defined)

```cpp
int begin(void)
void begin(IPAddress local_ip)
void begin(IPAddress local_ip, IPAddress dns_server)
void begin(IPAddress local_ip, IPAddress dns_server, IPAddress gateway)
void begin(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet)
```

Initialize using WIZ550io's built-in MAC address.

#### Network Management

```cpp
int maintain()
```

Maintain DHCP lease. Call regularly in loop(). Returns DHCP status codes:

-   `DHCP_CHECK_NONE`: No action taken
-   `DHCP_CHECK_RENEW_OK`: Lease renewed successfully
-   `DHCP_CHECK_REBIND_OK`: Lease rebound successfully
-   `DHCP_CHECK_RENEW_FAIL`: Lease renewal failed
-   `DHCP_CHECK_REBIND_FAIL`: Lease rebinding failed

#### Network Information

```cpp
IPAddress localIP()        // Get current IP address
IPAddress subnetMask()     // Get current subnet mask
IPAddress gatewayIP()      // Get current gateway IP
IPAddress dnsServerIP()    // Get current DNS server IP
char* dnsDomainName()      // Get DNS domain name (from DHCP)
char* hostName()           // Get host name (from DHCP)
```

### EthernetClient

TCP client class for establishing outbound connections.

#### Constructor

```cpp
EthernetClient(EthernetClass* eth, EthernetChip* chip)
EthernetClient(EthernetClass* eth, EthernetChip* chip, uint8_t sock)
```

#### Connection Methods

```cpp
int connect(IPAddress ip, uint16_t port)
int connect(const char* host, uint16_t port)
```

Establish TCP connection. Returns 1 if successful, 0 if failed.

#### Data Methods

```cpp
// Writing data
size_t write(uint8_t byte)
size_t write(const uint8_t* buf, size_t size)

// Reading data
int available()                              // Bytes available to read
int read()                                   // Read single byte
int read(uint8_t* buf, size_t size)         // Read multiple bytes
int peek()                                   // Peek at next byte

// Stream control
void flush()                                 // Flush outgoing data
void stop()                                  // Close connection
```

#### Status Methods

```cpp
uint8_t connected()       // Check if connected
uint8_t status()          // Get raw socket status
operator bool()           // Boolean conversion
```

### EthernetServer

TCP server class for accepting incoming connections.

#### Constructor

```cpp
EthernetServer(EthernetClass* eth, EthernetChip* chip, uint16_t port)
```

#### Server Methods

```cpp
void begin()                    // Start listening
EthernetClient available()      // Get client with data available
size_t write(uint8_t byte)      // Broadcast to all clients
size_t write(const uint8_t* buf, size_t size)  // Broadcast to all clients
```

### EthernetUDP

UDP communication class with multicast support.

#### Constructor

```cpp
EthernetUDP(EthernetClass* eth, EthernetChip* chip)
```

#### Socket Management

```cpp
uint8_t begin(uint16_t port)    // Start UDP on port
void stop()                     // Stop UDP
```

#### Sending Packets

```cpp
int beginPacket(IPAddress ip, uint16_t port)      // Start packet to IP
int beginPacket(const char* host, uint16_t port)  // Start packet to hostname
size_t write(uint8_t byte)                        // Add byte to packet
size_t write(const uint8_t* buffer, size_t size)  // Add data to packet
int endPacket()                                   // Send packet
```

#### Receiving Packets

```cpp
int parsePacket()                               // Process incoming packet
int available()                                 // Bytes available in packet
int read()                                      // Read single byte
int read(unsigned char* buffer, size_t len)     // Read multiple bytes
int read(char* buffer, size_t len)              // Read as characters
int peek()                                      // Peek at next byte
void flush()                                    // Finish reading packet
```

#### Packet Information

```cpp
IPAddress remoteIP()        // Get sender's IP address
uint16_t remotePort()       // Get sender's port
```

#### Multicast Support

```cpp
uint8_t beginMulticast(IPAddress multicast_ip, uint16_t port)  // Start multicast
int joinMulticastGroup(IPAddress group_ip)                     // Join group
int leaveMulticastGroup(IPAddress group_ip)                    // Leave group
bool isMulticastGroup(IPAddress ip)                            // Check if IP is multicast
```

## Chip Interface Classes

### EthernetChip (Abstract Base)

Abstract interface for WIZnet chip implementations.

#### Key Methods

```cpp
bool init()                                    // Initialize chip
bool linkActive()                              // Check physical link
uint8_t getChipType()                          // Get chip type ID
void swReset()                                 // Software reset
uint8_t getCSPin()                             // Get chip select pin
```

#### Network Configuration

```cpp
void setGatewayIp(uint8_t* addr)              // Set gateway IP
void getGatewayIp(uint8_t* addr)              // Get gateway IP
void setSubnetMask(uint8_t* addr)             // Set subnet mask
void getSubnetMask(uint8_t* addr)             // Get subnet mask
void setMACAddress(uint8_t* addr)             // Set MAC address
void getMACAddress(uint8_t* addr)             // Get MAC address
void setIPAddress(uint8_t* addr)              // Set IP address
void getIPAddress(uint8_t* addr)              // Get IP address
```

### W5500

Concrete implementation for W5500 chips.

#### Constructor

```cpp
W5500(uint8_t cs_pin)
```

#### W5500-Specific Features

-   Enhanced SPI performance
-   Improved socket buffer management
-   Advanced PHY configuration options

## DHCP Classes

### DhcpClass

DHCP client implementation for automatic network configuration.

#### Key Methods

```cpp
int beginWithDHCP(uint8_t* mac, unsigned long timeout = 60000, unsigned long responseTimeout = 5000)
int checkLease()                              // Check and maintain lease
IPAddress getLocalIp()                        // Get assigned IP
IPAddress getSubnetMask()                     // Get subnet mask
IPAddress getGatewayIp()                      // Get gateway IP
IPAddress getDhcpServerIp()                   // Get DHCP server IP
IPAddress getDnsServerIp()                    // Get DNS server IP
char* getDnsDomainName()                      // Get domain name
char* getHostName()                           // Get host name
```

## DNS Classes

### DNSClient

DNS resolution client for hostname-to-IP conversion.

#### Constructor

```cpp
DNSClient(EthernetClass* eth, EthernetChip* chip)
```

#### Methods

```cpp
void begin(IPAddress dns_server)              // Set DNS server
int getHostByName(const char* hostname, IPAddress& result)  // Resolve hostname
```

## Utility Functions and Macros

### Socket Functions (in chips/utility/socket.h)

```cpp
uint8_t socket(EthernetChip* chip, SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag)
void close(EthernetChip* chip, SOCKET s)
uint8_t connect(EthernetChip* chip, SOCKET s, uint8_t* addr, uint16_t port)
void disconnect(EthernetChip* chip, SOCKET s)
uint8_t listen(EthernetChip* chip, SOCKET s)
uint16_t send(EthernetChip* chip, SOCKET s, const uint8_t* buf, uint16_t len)
int16_t recv(EthernetChip* chip, SOCKET s, uint8_t* buf, int16_t len)
uint16_t peek(EthernetChip* chip, SOCKET s, uint8_t* buf)
uint16_t sendto(EthernetChip* chip, SOCKET s, const uint8_t* buf, uint16_t len, uint8_t* addr, uint16_t port)
uint16_t recvfrom(EthernetChip* chip, SOCKET s, uint8_t* buf, uint16_t len, uint8_t* addr, uint16_t* port)
void flush(EthernetChip* chip, SOCKET s)
```

## Constants and Enumerations

### Socket States (SnSR)

-   `SnSR::CLOSED`: Socket is closed
-   `SnSR::INIT`: Socket is initialized
-   `SnSR::LISTEN`: Socket is listening (TCP server)
-   `SnSR::ESTABLISHED`: TCP connection established
-   `SnSR::CLOSE_WAIT`: Connection closing (peer initiated)
-   `SnSR::FIN_WAIT`: Connection closing (local initiated)

### Socket Modes (SnMR)

-   `SnMR::CLOSE`: Close socket
-   `SnMR::TCP`: TCP mode
-   `SnMR::UDP`: UDP mode
-   `SnMR::IPRAW`: IP raw mode
-   `SnMR::MACRAW`: MAC raw mode
-   `SnMR::PPPOE`: PPPoE mode

### DHCP Status Codes

-   `DHCP_CHECK_NONE`: No DHCP action
-   `DHCP_CHECK_RENEW_OK`: Lease renewed successfully
-   `DHCP_CHECK_RENEW_FAIL`: Lease renewal failed
-   `DHCP_CHECK_REBIND_OK`: Lease rebound successfully
-   `DHCP_CHECK_REBIND_FAIL`: Lease rebinding failed

## Error Handling

### Common Return Values

-   `1` or `true`: Success
-   `0` or `false`: Failure or no data available
-   `-1`: Error condition or no data available (for read operations)

### Connection States

Use `EthernetClient::status()` to get detailed socket state information for debugging connection issues.

### DHCP Troubleshooting

Monitor `EthernetClass::maintain()` return values to detect and handle DHCP lease issues proactively.
