/**
 * @file EthernetServer.h
 * @brief TCP server implementation for Ethernet3 library
 *
 * This file provides the EthernetServer class which implements a TCP server
 * that can listen for incoming connections and accept multiple simultaneous
 * client connections.
 */

#ifndef ethernetserver_h
#define ethernetserver_h

#include "Ethernet3.h"
#include "EthernetClient.h"
#include "Server.h"
#include "chips/EthernetChip.h"
#include "chips/utility/socket.h"

#ifdef ETHERNET_BACKWARDS_COMPATIBILITY
// Forward declarations for global instances
extern class W5500 defaultChip;
extern class EthernetClass Ethernet;
#endif

class EthernetClient;

/**
 * @brief TCP server class for accepting incoming network connections
 *
 * EthernetServer provides a TCP server implementation that can bind to a
 * specific port and accept incoming client connections. It's compatible with
 * the standard Arduino Server interface while providing enhanced functionality.
 *
 * Key features:
 * - Support for multiple simultaneous client connections
 * - Automatic socket management and reuse
 * - Enhanced connection handling and error recovery
 * - Compatible with standard Arduino Server interface
 *
 * Unlike the standard Arduino Ethernet library, this implementation:
 * - Uses an abstract chip interface for better hardware abstraction
 * - Provides better socket reuse and connection state management
 * - Supports more reliable multi-client handling
 */
class EthernetServer : public Server {
   private:
    EthernetClass* _ethernet;  ///< Pointer to the Ethernet class instance
    EthernetChip* _chip;       ///< Pointer to the Ethernet chip interface
    uint16_t _port;            ///< Port number to listen on

    /**
     * @brief Accept pending connections on listening sockets
     *
     * Internal function that checks all sockets for incoming connections
     * and transitions them from LISTEN to ESTABLISHED state when clients connect.
     */
    void accept();

   public:
    /**
     * @brief Construct a new EthernetServer object
     * @param eth Pointer to EthernetClass instance
     * @param chip Pointer to EthernetChip interface
     * @param port Port number to listen on
     *
     * Creates a server that will listen on the specified port for incoming
     * TCP connections.
     */
    EthernetServer(EthernetClass* eth, EthernetChip* chip, uint16_t port);

#ifdef ETHERNET_BACKWARDS_COMPATIBILITY
    // Backwards compatibility constructor that uses global instances
    EthernetServer(uint16_t port);
#endif

    /**
     * @brief Get an available client connection
     * @return EthernetClient object representing a connected client, or invalid client if none
     * available
     *
     * Checks for clients with data available or newly connected clients.
     * Returns an EthernetClient object that can be used to communicate with the client.
     * If no clients are available, returns an invalid client (evaluates to false).
     */
    EthernetClient available();

    /**
     * @brief Start listening for incoming connections
     *
     * Initializes the server to listen for incoming connections on the specified port.
     * Multiple sockets may be allocated to support simultaneous connections.
     */
    virtual void begin();

    /**
     * @brief Write a single byte to all connected clients
     * @param byte Byte to write
     * @return Number of bytes written
     *
     * Sends a single byte to all currently connected clients.
     * Useful for broadcasting data to multiple clients simultaneously.
     */
    virtual size_t write(uint8_t byte);

    /**
     * @brief Write multiple bytes to all connected clients
     * @param buf Buffer containing data to write
     * @param size Number of bytes to write
     * @return Number of bytes written
     *
     * Sends the specified data to all currently connected clients.
     * Useful for broadcasting data to multiple clients simultaneously.
     */
    virtual size_t write(const uint8_t* buf, size_t size);

    /**
     * @brief Inherit write functions from Print class
     */
    using Print::write;
};

#endif
