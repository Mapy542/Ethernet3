/**
 * @file EthernetClient.h
 * @brief TCP client implementation for Ethernet3 library
 * 
 * This file provides the EthernetClient class which implements a TCP client
 * compatible with the Arduino Client interface. It supports connecting to
 * remote servers, sending and receiving data over TCP connections.
 */

#ifndef ethernetclient_h
#define ethernetclient_h

#include "Client.h"
#include "Dns.h"
#include "Ethernet3.h"
#include "IPAddress.h"
#include "chips/EthernetChip.h"
#include "chips/utility/socket.h"

/**
 * @brief TCP client class for establishing outbound network connections
 * 
 * EthernetClient provides a TCP client implementation that can connect to
 * remote servers, send data, and receive responses. It's compatible with
 * the standard Arduino Client interface, making it easy to port existing code.
 * 
 * Key features:
 * - DNS resolution support for connecting by hostname
 * - Automatic socket management
 * - Compatible with standard Arduino Client interface
 * - Enhanced error handling and status reporting
 * 
 * Unlike the standard Arduino Ethernet library, this implementation:
 * - Uses an abstract chip interface for better hardware abstraction
 * - Provides enhanced connection status reporting
 * - Supports multiple simultaneous connections more reliably
 */
class EthernetClient : public Client {
   private:
    EthernetClass *_ethernet;  ///< Pointer to the Ethernet class instance
    EthernetChip *_chip;       ///< Pointer to the Ethernet chip interface
   public:
    /**
     * @brief Construct a new EthernetClient object
     * @param eth Pointer to EthernetClass instance
     * @param chip Pointer to EthernetChip interface
     * 
     * Creates a new client instance that will use the specified Ethernet
     * and chip instances for network operations.
     */
    EthernetClient(EthernetClass *eth, EthernetChip *chip);
    
    /**
     * @brief Construct a new EthernetClient object with specific socket
     * @param eth Pointer to EthernetClass instance
     * @param chip Pointer to EthernetChip interface
     * @param sock Socket number to use for this client
     * 
     * Creates a new client instance using a specific socket number.
     * This is typically used internally by EthernetServer when accepting connections.
     */
    EthernetClient(EthernetClass *eth, EthernetChip *chip, uint8_t sock);

    /**
     * @brief Get connection status
     * @return Socket status code (SnSR_ESTABLISHED, SnSR_CLOSED, etc.)
     * 
     * Returns the current status of the underlying socket. Useful for
     * determining the exact state of the connection.
     */
    uint8_t status();
    
    /**
     * @brief Connect to a server by IP address
     * @param ip IP address of the server to connect to
     * @param port Port number to connect to
     * @return 1 if connection successful, 0 if failed
     * 
     * Establishes a TCP connection to the specified IP address and port.
     * This is a blocking operation that will return once the connection
     * is established or fails.
     */
    virtual int connect(IPAddress ip, uint16_t port);
    
    /**
     * @brief Connect to a server by hostname
     * @param host Hostname or domain name to connect to
     * @param port Port number to connect to
     * @return 1 if connection successful, 0 if failed
     * 
     * Establishes a TCP connection to the specified hostname and port.
     * This function performs DNS resolution to convert the hostname to
     * an IP address before connecting. This is a blocking operation.
     */
    virtual int connect(const char *host, uint16_t port);
    
    /**
     * @brief Write a single byte
     * @param byte Byte to write
     * @return Number of bytes written (1 if successful, 0 if failed)
     * 
     * Sends a single byte over the connection. The data is typically
     * buffered by the chip and may not be sent immediately.
     */
    virtual size_t write(uint8_t byte);
    
    /**
     * @brief Write multiple bytes
     * @param buf Buffer containing data to write
     * @param size Number of bytes to write
     * @return Number of bytes actually written
     * 
     * Sends multiple bytes over the connection. May write fewer bytes
     * than requested if the send buffer is full.
     */
    virtual size_t write(const uint8_t *buf, size_t size);
    
    /**
     * @brief Get number of bytes available for reading
     * @return Number of bytes available in the receive buffer
     * 
     * Returns the number of bytes that can be read immediately without
     * blocking. Returns 0 if no data is available.
     */
    virtual int available();
    
    /**
     * @brief Read a single byte
     * @return Byte value (0-255) or -1 if no data available
     * 
     * Reads and removes one byte from the receive buffer. Returns -1
     * if no data is available.
     */
    virtual int read();
    
    /**
     * @brief Read multiple bytes
     * @param buf Buffer to store received data
     * @param size Maximum number of bytes to read
     * @return Number of bytes actually read
     * 
     * Reads up to 'size' bytes from the receive buffer into the provided
     * buffer. Returns the actual number of bytes read, which may be less
     * than requested.
     */
    virtual int read(uint8_t *buf, size_t size);
    
    /**
     * @brief Peek at the next byte without removing it
     * @return Next byte value (0-255) or -1 if no data available
     * 
     * Returns the next byte in the receive buffer without removing it.
     * Subsequent calls to read() or peek() will return the same byte.
     */
    virtual int peek();
    
    /**
     * @brief Flush outgoing data
     * 
     * Ensures that any buffered outgoing data is sent immediately.
     * This is a blocking operation that waits until all data is transmitted.
     */
    virtual void flush();
    
    /**
     * @brief Close the connection
     * 
     * Closes the TCP connection and releases the socket for reuse.
     * After calling stop(), the client cannot be used until a new
     * connection is established.
     */
    virtual void stop();
    
    /**
     * @brief Check if the client is connected
     * @return Non-zero if connected, 0 if disconnected
     * 
     * Returns whether the client has an active connection to a server.
     * This checks both the socket state and available data.
     */
    virtual uint8_t connected();
    
    /**
     * @brief Boolean conversion operator
     * @return true if connected, false if disconnected
     * 
     * Allows the client to be used in boolean expressions.
     * Returns true if there's an active connection or data available.
     */
    virtual operator bool();
    
    /**
     * @brief Equality comparison operator
     * @param other EthernetClient to compare with
     * @return true if clients use the same socket, false otherwise
     * 
     * Compares two clients to determine if they represent the same connection.
     */
    virtual bool operator==(const EthernetClient &other);
    
    /**
     * @brief Inequality comparison operator
     * @param rhs EthernetClient to compare with
     * @return true if clients use different sockets, false otherwise
     * 
     * Compares two clients to determine if they represent different connections.
     */
    virtual bool operator!=(const EthernetClient &rhs) { return !this->operator==(rhs); };

    /**
     * @brief Allow EthernetServer to access private members
     */
    friend class EthernetServer;

    /**
     * @brief Inherit write functions from Print class
     */
    using Print::write;

   private:
    static uint16_t _srcport; ///< Static source port counter for new connections
    uint8_t _sock;           ///< Socket number used by this client
};

#endif
