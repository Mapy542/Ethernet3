/**
 * @file EthernetUdp2.h
 * @brief UDP communication implementation for Ethernet3 library
 * 
 * This file provides the EthernetUDP class for sending and receiving UDP packets.
 * This version offers enhanced UDP functionality with multicast support and
 * improved packet handling over the standard Arduino UDP implementation.
 * 
 * UDP (User Datagram Protocol) is a connectionless protocol that is fast but
 * has important limitations:
 * 1) UDP does not guarantee packet order - packets may arrive out of sequence
 * 2) UDP does not guarantee delivery - packets can be lost without notification
 * 3) UDP provides no error correction or flow control
 * 
 * For more information about UDP limitations and use cases, see:
 * http://www.cafeaulait.org/course/week12/35.html
 *
 * MIT License:
 * Copyright (c) 2008 Bjoern Hartmann
 * 
 * - 10 Apr. 2015
 * Added support for Arduino Ethernet Shield 2
 * by Arduino.org team
 * 
 * Enhanced for Ethernet3 library with:
 * - Abstract chip interface support
 * - Multicast functionality
 * - Improved error handling
 * - Better packet management
 */

#ifndef ethernetudp_h
#define ethernetudp_h

#include <Udp.h>

// #include "Dns.h"
#include "Ethernet3.h"
#include "chips/EthernetChip.h"
#include "chips/utility/socket.h"

class DNSClient;      // Forward declaration to avoid circular dependency
class EthernetClass;  // Forward declaration to avoid circular dependency

/**
 * @brief UDP communication class for sending and receiving UDP packets
 * 
 * EthernetUDP provides a complete UDP implementation compatible with the
 * standard Arduino UDP interface while offering enhanced functionality:
 * 
 * Key features:
 * - Standard UDP send/receive operations
 * - DNS hostname resolution for outgoing packets
 * - Multicast group support (join/leave multicast groups)
 * - Enhanced packet buffering and management
 * - Compatible with standard Arduino UDP interface
 * 
 * Unlike the standard Arduino Ethernet library, this implementation:
 * - Uses an abstract chip interface for better hardware abstraction
 * - Provides comprehensive multicast support
 * - Offers improved error handling and packet state management
 * - Supports enhanced buffering mechanisms
 */
class EthernetUDP : public UDP {
   private:
    EthernetClass* _ethernet;  ///< Pointer to the Ethernet class instance
    EthernetChip* _chip;       ///< Pointer to the Ethernet chip interface
    uint8_t _sock;             ///< Socket ID for the UDP connection
    uint16_t _port;            ///< Local port to listen on
    IPAddress _remoteIP;       ///< Remote IP address for the current incoming packet
    uint16_t _remotePort;      ///< Remote port for the current incoming packet
    uint16_t _offset;          ///< Offset into the packet being sent
    uint16_t _remaining;       ///< Remaining bytes of incoming packet yet to be processed

   public:
    /**
     * @brief Construct a new EthernetUDP object
     * @param eth Pointer to EthernetClass instance
     * @param chip Pointer to EthernetChip interface
     * 
     * Creates a UDP instance that will use the specified Ethernet
     * and chip instances for network operations.
     */
    EthernetUDP(EthernetClass* eth, EthernetChip* chip);
    /**
     * @brief Initialize UDP and start listening on specified port
     * @param port UDP port to listen on
     * @return 1 if successful, 0 if no sockets available
     * 
     * Initializes the UDP socket and binds it to the specified port.
     * The socket will be ready to send and receive UDP packets.
     */
    virtual uint8_t begin(uint16_t port);
    
    /**
     * @brief Stop UDP communication and release the socket
     * 
     * Closes the UDP socket and releases it for reuse by other UDP instances.
     * After calling stop(), begin() must be called again before the UDP
     * instance can be used.
     */
    virtual void stop();

    // ===== Sending UDP packets =====

    /**
     * @brief Start building a packet to send to specified IP and port
     * @param ip Destination IP address
     * @param port Destination port number
     * @return 1 if successful, 0 if there was a problem with the IP address or port
     * 
     * Initializes a new outgoing UDP packet. After calling this function,
     * use write() methods to add data to the packet, then call endPacket()
     * to send it.
     */
    virtual int beginPacket(IPAddress ip, uint16_t port);
    
    /**
     * @brief Start building a packet to send to specified hostname and port
     * @param host Destination hostname (will be resolved via DNS)
     * @param port Destination port number
     * @return 1 if successful, 0 if hostname resolution failed
     * 
     * Performs DNS resolution to convert hostname to IP address, then
     * initializes a new outgoing UDP packet. This is a blocking operation
     * during DNS resolution.
     */
    virtual int beginPacket(const char* host, uint16_t port);
    
    /**
     * @brief Finish and send the current packet
     * @return 1 if packet was sent successfully, 0 if there was an error
     * 
     * Completes the current outgoing packet and transmits it to the
     * destination specified in beginPacket(). The packet buffer is
     * cleared after transmission.
     */
    virtual int endPacket();
    
    /**
     * @brief Write a single byte to the current packet
     * @param byte Byte to add to the packet
     * @return Number of bytes written (1 if successful, 0 if failed)
     * 
     * Adds a single byte to the current outgoing packet buffer.
     * Must be called between beginPacket() and endPacket().
     */
    virtual size_t write(uint8_t byte);
    
    /**
     * @brief Write multiple bytes to the current packet
     * @param buffer Data buffer to add to the packet
     * @param size Number of bytes to write
     * @return Number of bytes actually written
     * 
     * Adds the specified data to the current outgoing packet buffer.
     * Must be called between beginPacket() and endPacket().
     */
    virtual size_t write(const uint8_t* buffer, size_t size);

    /**
     * @brief Inherit write functions from Print class
     */
    using Print::write;

    // ===== Receiving UDP packets =====

    /**
     * @brief Start processing the next available incoming packet
     * @return Size of the packet in bytes, or 0 if no packets are available
     * 
     * Checks for incoming UDP packets and prepares the first available
     * packet for reading. Returns the size of the packet, which can then
     * be read using read() methods. Must be called before attempting to
     * read packet data.
     */
    virtual int parsePacket();
    
    /**
     * @brief Get number of bytes remaining in the current packet
     * @return Number of bytes available for reading
     * 
     * Returns the number of bytes that can still be read from the current
     * incoming packet. Returns 0 if no packet is being processed or all
     * data has been read.
     */
    virtual int available();
    
    /**
     * @brief Read a single byte from the current packet
     * @return Byte value (0-255) or -1 if no data available
     * 
     * Reads and removes one byte from the current incoming packet.
     * Returns -1 if no packet is being processed or all data has been read.
     */
    virtual int read();
    
    /**
     * @brief Read multiple bytes from the current packet
     * @param buffer Buffer to store received data
     * @param len Maximum number of bytes to read
     * @return Number of bytes actually read
     * 
     * Reads up to 'len' bytes from the current incoming packet into the
     * provided buffer. Returns the actual number of bytes read.
     */
    virtual int read(unsigned char* buffer, size_t len);
    
    /**
     * @brief Read multiple characters from the current packet
     * @param buffer Character buffer to store received data
     * @param len Maximum number of characters to read
     * @return Number of characters actually read
     * 
     * Convenience method that reads packet data as characters.
     * Equivalent to read((unsigned char*)buffer, len).
     */
    virtual int read(char* buffer, size_t len) { return read((unsigned char*)buffer, len); };
    
    /**
     * @brief Peek at the next byte without removing it
     * @return Next byte value (0-255) or -1 if no data available
     * 
     * Returns the next byte in the current packet without consuming it.
     * Subsequent calls to read() or peek() will return the same byte.
     */
    virtual int peek();
    
    /**
     * @brief Finish reading the current packet
     * 
     * Discards any remaining data in the current incoming packet and
     * prepares for the next packet. Call this when you're done reading
     * a packet to clean up the receive state.
     */
    virtual void flush();

    // ===== Packet Information =====

    /**
     * @brief Get the IP address of the sender of the current packet
     * @return IP address of the packet sender
     * 
     * Returns the source IP address of the current incoming packet.
     * Only valid after parsePacket() has been called successfully.
     */
    virtual IPAddress remoteIP() { return _remoteIP; };
    
    /**
     * @brief Get the port number of the sender of the current packet
     * @return Port number of the packet sender
     * 
     * Returns the source port number of the current incoming packet.
     * Only valid after parsePacket() has been called successfully.
     */
    virtual uint16_t remotePort() { return _remotePort; };

    // ===== Multicast Support =====

    /**
     * @brief Begin UDP multicast on specified group and port
     * @param multicast_ip Multicast group IP address (224.0.0.0/4 range)
     * @param port UDP port to listen on
     * @return 1 if successful, 0 if failed
     * 
     * Initializes the UDP socket for multicast communication and joins
     * the specified multicast group. The IP address must be in the
     * multicast range (224.0.0.0 to 239.255.255.255).
     */
    virtual uint8_t beginMulticast(IPAddress multicast_ip, uint16_t port);

    /**
     * @brief Join a multicast group
     * @param group_ip Multicast group IP address
     * @return 1 if successful, 0 if failed
     * 
     * Joins the specified multicast group, allowing reception of packets
     * sent to that group. The socket must already be initialized with
     * begin() or beginMulticast().
     */
    virtual int joinMulticastGroup(IPAddress group_ip);

    /**
     * @brief Leave a multicast group
     * @param group_ip Multicast group IP address
     * @return 1 if successful, 0 if failed
     * 
     * Leaves the specified multicast group, stopping reception of packets
     * sent to that group. Other multicast groups remain active.
     */
    virtual int leaveMulticastGroup(IPAddress group_ip);

    /**
     * @brief Check if IP address is in multicast range
     * @param ip IP address to check
     * @return true if IP is multicast (224.0.0.0/4), false otherwise
     * 
     * Utility function to validate if an IP address is in the valid
     * multicast address range (224.0.0.0 to 239.255.255.255).
     */
    virtual bool isMulticastGroup(IPAddress ip);

   private:
    /**
     * @brief Calculate multicast MAC address from IP
     * @param ip Multicast IP address
     * @param mac Output buffer for MAC address (6 bytes)
     * 
     * Converts a multicast IP address to the corresponding multicast
     * MAC address according to RFC 1112. The MAC address follows the
     * pattern 01:00:5E:xx:xx:xx where xx:xx:xx are the lower 23 bits
     * of the IP address.
     */
    void calculateMulticastMAC(IPAddress ip, uint8_t* mac);

    /**
     * @brief Configure socket for multicast reception
     * @param group_ip Multicast group IP address
     * @param port UDP port
     * 
     * Internal function that configures the Ethernet chip's socket
     * and MAC filter settings for multicast packet reception.
     */
    void configureMulticastSocket(IPAddress group_ip, uint16_t port);
};

#endif
