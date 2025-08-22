
/**
 * @file EthernetClient.cpp
 * @brief Implementation of TCP client functionality for Ethernet3 library
 *
 * This file implements the EthernetClient class providing TCP client
 * capabilities including connection establishment, data transmission,
 * and connection management with enhanced features over the standard
 * Arduino Ethernet library.
 */

extern "C" {
#include "string.h"
}

#include "EthernetClient.h"

/// Static source port counter - increments for each new connection
uint16_t EthernetClient::_srcport = 1024;

/**
 * @brief Construct a new EthernetClient without a specific socket
 * @param eth Pointer to EthernetClass instance
 * @param chip Pointer to EthernetChip interface
 *
 * Creates a client that will allocate a socket when connect() is called.
 */
EthernetClient::EthernetClient(EthernetClass* eth, EthernetChip* chip)
    : _ethernet(eth), _chip(chip), _sock(MAX_SOCK_NUM) {}

/**
 * @brief Construct a new EthernetClient with a specific socket
 * @param eth Pointer to EthernetClass instance
 * @param chip Pointer to EthernetChip interface
 * @param sock Socket number to use
 *
 * Creates a client using the specified socket. Used internally by EthernetServer.
 */
EthernetClient::EthernetClient(EthernetClass* eth, EthernetChip* chip, uint8_t sock)
    : _ethernet(eth), _chip(chip), _sock(sock) {}

#ifdef ETHERNET_BACKWARDS_COMPATIBILITY
EthernetClient::EthernetClient() : _ethernet(&Ethernet), _chip(&defaultChip), _sock(MAX_SOCK_NUM) {}

EthernetClient::EthernetClient(uint8_t sock)
    : _ethernet(&Ethernet), _chip(&defaultChip), _sock(sock) {}
#endif

/**
 * @brief Connect to a server using hostname resolution
 * @param host Hostname or domain name
 * @param port Port number to connect to
 * @return 1 if successful, 0 if failed
 *
 * Performs DNS resolution to convert hostname to IP address, then
 * establishes a TCP connection. This is a blocking operation.
 */
int EthernetClient::connect(const char* host, uint16_t port) {
    // Look up the host first
    int ret = 0;
    DNSClient dns(_ethernet, _chip);
    IPAddress remote_addr;

    dns.begin(_ethernet->dnsServerIP());
    ret = dns.getHostByName(host, remote_addr);
    if (ret == 1) {
        return connect(remote_addr, port);
    } else {
        return ret;
    }
}

/**
 * @brief Connect to a server by IP address
 * @param ip IP address to connect to
 * @param port Port number to connect to
 * @return 1 if successful, 0 if failed
 *
 * Establishes a TCP connection to the specified IP address and port.
 * The function allocates an available socket, binds to an ephemeral port,
 * and waits for the connection to be established.
 */
int EthernetClient::connect(IPAddress ip, uint16_t port) {
    if (_sock != MAX_SOCK_NUM) return 0;

    for (int i = 0; i < MAX_SOCK_NUM; i++) {
        uint8_t s = _chip->readSnSR(i);
        if (s == SnSR::CLOSED || s == SnSR::FIN_WAIT || s == SnSR::CLOSE_WAIT) {
            _sock = i;
            break;
        }
    }

    if (_sock == MAX_SOCK_NUM) return 0;

    _srcport++;
    if (_srcport == 0) _srcport = 1024;
    socket(_chip, _sock, SnMR::TCP, _srcport, 0);

    if (!::connect(_chip, _sock, rawIPAddress(ip), port)) {
        _sock = MAX_SOCK_NUM;
        return 0;
    }

    while (status() != SnSR::ESTABLISHED) {
        delay(1);
        if (status() == SnSR::CLOSED) {
            _sock = MAX_SOCK_NUM;
            return 0;
        }
    }

    return 1;
}

/**
 * @brief Write a single byte to the connection
 * @param b Byte to write
 * @return Number of bytes written (1 if successful, 0 if failed)
 *
 * Sends a single byte over the TCP connection.
 */
size_t EthernetClient::write(uint8_t b) { return write(&b, 1); }

/**
 * @brief Write multiple bytes to the connection
 * @param buf Buffer containing data to write
 * @param size Number of bytes to write
 * @return Number of bytes written, or 0 if error
 *
 * Sends the specified data over the TCP connection. Sets write error
 * flag if the socket is invalid or send operation fails.
 */
size_t EthernetClient::write(const uint8_t* buf, size_t size) {
    if (_sock == MAX_SOCK_NUM) {
        setWriteError();
        return 0;
    }
    if (!send(_chip, _sock, buf, size)) {
        setWriteError();
        return 0;
    }
    return size;
}

/**
 * @brief Get number of bytes available for reading
 * @return Number of bytes in receive buffer, or 0 if not connected
 *
 * Returns the number of bytes that can be read from the receive buffer
 * without blocking.
 */
int EthernetClient::available() {
    if (_sock != MAX_SOCK_NUM) return _chip->getRXReceivedSize(_sock);
    return 0;
}

/**
 * @brief Read a single byte from the connection
 * @return Byte value (0-255) or -1 if no data available
 *
 * Reads and removes one byte from the receive buffer.
 */
int EthernetClient::read() {
    uint8_t b;
    if (recv(_chip, _sock, &b, 1) > 0) {
        // recv worked
        return b;
    } else {
        // No data available
        return -1;
    }
}

/**
 * @brief Read multiple bytes from the connection
 * @param buf Buffer to store received data
 * @param size Maximum number of bytes to read
 * @return Number of bytes actually read
 *
 * Reads up to 'size' bytes from the receive buffer.
 */
int EthernetClient::read(uint8_t* buf, size_t size) { return recv(_chip, _sock, buf, size); }

/**
 * @brief Peek at the next byte without removing it
 * @return Next byte value (0-255) or -1 if no data available
 *
 * Returns the next byte in the receive buffer without consuming it.
 * Subsequent calls to read() or peek() will return the same byte.
 */
int EthernetClient::peek() {
    uint8_t b;
    // Unlike recv, peek doesn't check to see if there's any data available, so we must
    if (!available()) return -1;
    ::peek(_chip, _sock, &b);
    return b;
}

/**
 * @brief Flush outgoing data
 *
 * Ensures all buffered outgoing data is transmitted. This is a blocking
 * operation that waits until transmission is complete.
 */
void EthernetClient::flush() { ::flush(_chip, _sock); }

/**
 * @brief Close the connection gracefully
 *
 * Attempts to close the TCP connection gracefully by sending a FIN packet.
 * Waits up to 1 second for graceful closure, then forces closure if needed.
 * Releases the socket for reuse.
 */
void EthernetClient::stop() {
    if (_sock == MAX_SOCK_NUM) return;

    // attempt to close the connection gracefully (send a FIN to other side)
    disconnect(_chip, _sock);
    unsigned long start = millis();

    // wait a second for the connection to close
    while (status() != SnSR::CLOSED && millis() - start < 1000) delay(1);

    // if it hasn't closed, close it forcefully
    if (status() != SnSR::CLOSED) close(_chip, _sock);

    _ethernet->_server_port[_sock] = 0;
    _sock = MAX_SOCK_NUM;
}

/**
 * @brief Check if the client is connected
 * @return Non-zero if connected, 0 if disconnected
 *
 * Returns whether the client has an active connection. Considers the
 * connection active if the socket is established and not in a closing state,
 * or if there's still data available to read.
 */
uint8_t EthernetClient::connected() {
    if (_sock == MAX_SOCK_NUM) return 0;

    uint8_t s = status();
    return !(s == SnSR::LISTEN || s == SnSR::CLOSED || s == SnSR::FIN_WAIT ||
             (s == SnSR::CLOSE_WAIT && !available()));
}

/**
 * @brief Get the raw socket status
 * @return Socket status code from the chip
 *
 * Returns the current status of the underlying socket as reported by
 * the Ethernet chip. Useful for detailed connection state analysis.
 */
uint8_t EthernetClient::status() {
    if (_sock == MAX_SOCK_NUM) return SnSR::CLOSED;
    return _chip->readSnSR(_sock);
}

/**
 * @brief Boolean conversion operator
 * @return true if client has a valid socket, false otherwise
 *
 * Allows the client to be used in boolean expressions like if(client).
 * Returns true if the client has been assigned a socket.
 */
EthernetClient::operator bool() { return _sock != MAX_SOCK_NUM; }

/**
 * @brief Equality comparison operator
 * @param rhs EthernetClient to compare with
 * @return true if both clients use the same valid socket
 *
 * Compares two clients to determine if they represent the same connection.
 * Both clients must have valid sockets and use the same socket number.
 */
bool EthernetClient::operator==(const EthernetClient& rhs) {
    return _sock == rhs._sock && _sock != MAX_SOCK_NUM && rhs._sock != MAX_SOCK_NUM;
}
