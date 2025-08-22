
/**
 * @file EthernetServer.cpp
 * @brief Implementation of TCP server functionality for Ethernet3 library
 * 
 * This file implements the EthernetServer class providing TCP server
 * capabilities including listening for connections, accepting clients,
 * and broadcasting data to multiple connected clients.
 */

extern "C" {
#include "string.h"
}

#include "EthernetServer.h"

/**
 * @brief Construct a new EthernetServer object
 * @param eth Pointer to EthernetClass instance
 * @param chip Pointer to EthernetChip interface
 * @param port Port number to listen on
 * 
 * Creates a server that will listen on the specified port.
 */
EthernetServer::EthernetServer(EthernetClass* eth, EthernetChip* chip, uint16_t port)
    : _ethernet(eth), _chip(chip), _port(port) {}

/**
 * @brief Start the server listening for connections
 * 
 * Initializes the server by finding an available socket and configuring
 * it to listen for incoming TCP connections on the specified port.
 * Only one socket is initially allocated; additional sockets are allocated
 * as needed when clients connect.
 */
void EthernetServer::begin() {
    for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
        EthernetClient client(_ethernet, _chip, sock);
        if (client.status() == SnSR::CLOSED) {
            socket(_chip, sock, SnMR::TCP, _port, 0);
            listen(_chip, sock);
            _ethernet->_server_port[sock] = _port;
            break;
        }
    }
}

/**
 * @brief Accept pending connections and maintain server state
 * 
 * Internal function that handles server maintenance tasks:
 * - Cleans up closed connections in CLOSE_WAIT state
 * - Ensures at least one socket is always listening for new connections
 * - Allocates additional listening sockets if none are available
 * 
 * This function is called automatically by available() and write() methods.
 */
void EthernetServer::accept() {
    int listening = 0;

    for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
        EthernetClient client(_ethernet, _chip, sock);

        if (_ethernet->_server_port[sock] == _port) {
            if (client.status() == SnSR::LISTEN) {
                listening = 1;
            } else if (client.status() == SnSR::CLOSE_WAIT && !client.available()) {
                client.stop();
            }
        }
    }

    if (!listening) {
        begin();
    }
}

/**
 * @brief Get a client with available data
 * @return EthernetClient object for a connected client with data, or invalid client if none
 * 
 * Checks all sockets associated with this server port for clients that have
 * data available to read. Returns the first client found with available data.
 * If no clients have data available, returns an invalid client object that
 * evaluates to false in boolean contexts.
 * 
 * This function also performs server maintenance by calling accept().
 * 
 * @note The function currently always returns the lowest-numbered socket with data.
 *       Future versions may implement round-robin selection for better fairness.
 */
EthernetClient EthernetServer::available() {
    accept();

    for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
        EthernetClient client(_ethernet, _chip, sock);
        if (_ethernet->_server_port[sock] == _port &&
            (client.status() == SnSR::ESTABLISHED || client.status() == SnSR::CLOSE_WAIT)) {
            if (client.available()) {
                // XXX: don't always pick the lowest numbered socket.
                return client;
            }
        }
    }

    return EthernetClient(_ethernet, _chip, MAX_SOCK_NUM);
}

/**
 * @brief Write a single byte to all connected clients
 * @param b Byte to write
 * @return Total number of bytes written across all clients
 * 
 * Broadcasts a single byte to all clients currently connected to this server.
 */
size_t EthernetServer::write(uint8_t b) { return write(&b, 1); }

/**
 * @brief Write data to all connected clients
 * @param buffer Data buffer to write
 * @param size Number of bytes to write
 * @return Total number of bytes written across all clients
 * 
 * Broadcasts the specified data to all clients currently connected and
 * established on this server. The return value is the sum of bytes written
 * to each individual client.
 * 
 * This function also performs server maintenance by calling accept().
 */
size_t EthernetServer::write(const uint8_t* buffer, size_t size) {
    size_t n = 0;

    accept();

    for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
        EthernetClient client(_ethernet, _chip, sock);

        if (_ethernet->_server_port[sock] == _port && client.status() == SnSR::ESTABLISHED) {
            n += client.write(buffer, size);
        }
    }

    return n;
}
