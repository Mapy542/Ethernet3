/*
 * EthernetChip.h - Abstract Ethernet chip interface for Ethernet3 library
 *
 * This file provides a common interface for different Ethernet chips
 * enabling support for W5100, W5500 and future chip variants.
 */

#ifndef ETHERNET_CHIP_H
#define ETHERNET_CHIP_H

#include <stdint.h>

#include "../hal/EthernetPlatform.h"

// Chip type constants
#define CHIP_TYPE_W5100 0x51
#define CHIP_TYPE_W5500 0x55

/**
 * Abstract base class for Ethernet chip implementations.
 * Specific chip classes (W5100, W5500) should inherit from this class.
 */
class EthernetChip {
   protected:
    EthernetPlatform* platform;  ///< Unified platform interface
    uint8_t cs_pin;              ///< Chip select pin

   public:
    /**
     * Constructor
     * @param platform_interface Pointer to unified platform implementation
     * @param chip_select_pin Chip select pin number
     */
    EthernetChip(EthernetPlatform* platform_interface, uint8_t chip_select_pin)
        : platform(platform_interface), cs_pin(chip_select_pin) {}

    virtual ~EthernetChip() {}

    /**
     * Initialize the chip
     * @return true if initialization successful, false otherwise
     */
    virtual bool init() = 0;

    /**
     * Check physical link status
     * @return true if link is active, false otherwise
     */
    virtual bool linkActive() = 0;

    /**
     * Get chip type identifier
     * @return Chip type constant (CHIP_TYPE_W5100, CHIP_TYPE_W5500, etc.)
     */
    virtual uint8_t getChipType() = 0;

    /**
     * Software reset the chip
     */
    virtual void swReset() = 0;

    /**
     * Get chip select pin
     * @return Chip select pin number
     */
    uint8_t getCSPin() const { return cs_pin; }

    /**
     * @brief Get the underlying platform interface
     *
     */
    EthernetPlatform* getPlatform() const { return platform; }

    // ---------------------------------------------------------------------
    // Common network configuration accessors (must be implemented)
    // ---------------------------------------------------------------------
    /** Set gateway IPv4 address (4 bytes) */
    virtual void setGatewayIp(const uint8_t* addr) = 0;
    /** Get gateway IPv4 address (4 bytes) */
    virtual void getGatewayIp(uint8_t* addr) = 0;
    /** Set subnet mask (4 bytes) */
    virtual void setSubnetMask(const uint8_t* addr) = 0;
    /** Get subnet mask (4 bytes) */
    virtual void getSubnetMask(uint8_t* addr) = 0;
    /** Set MAC address (6 bytes) */
    virtual void setMACAddress(const uint8_t* addr) = 0;
    /** Get MAC address (6 bytes) */
    virtual void getMACAddress(uint8_t* addr) = 0;
    /** Set IPv4 address (4 bytes) */
    virtual void setIPAddress(const uint8_t* addr) = 0;
    /** Get IPv4 address (4 bytes) */
    virtual void getIPAddress(uint8_t* addr) = 0;
    /** Set retransmission timeout (ms units per chip spec) */
    virtual void setRetransmissionTime(uint16_t timeout) = 0;
    /** Set retransmission retry count */
    virtual void setRetransmissionCount(uint8_t retry) = 0;

    // ---------------------------------------------------------------------
    // PHY / Link configuration (optional override where supported)
    // ---------------------------------------------------------------------
    /** Set raw PHY configuration register (only on chips supporting it). Default no-op. */
    virtual void setPHYCFGR(uint8_t /*val*/) { /* default: unsupported */ }
    /** Get raw PHY configuration register (return 0 if unsupported). */
    virtual uint8_t getPHYCFGR() { return 0; }

    // ---------------------------------------------------------------------
    // Socket level register / buffer helpers
    // These abstract minimal functionality required by higher layers.
    // ---------------------------------------------------------------------
    /** Maximum simultaneous sockets supported by the chip. */
    virtual uint8_t maxSockets() const = 0;

    /** Execute a socket command (OPEN, LISTEN, SEND, RECV, CLOSE...). */
    virtual void execSocketCommand(uint8_t socket, uint8_t cmd) = 0;

    /** Read socket status register. */
    virtual uint8_t readSocketStatus(uint8_t socket) = 0;
    /** Read socket interrupt flags (and optionally clear – chip impl decides). */
    virtual uint8_t readSocketInterrupt(uint8_t socket) = 0;
    /** Clear specific socket interrupt flags. */
    virtual void writeSocketInterrupt(uint8_t socket, uint8_t flags) = 0;

    // Basic socket configuration helpers (mode, source/destination addressing)
    /** Set socket mode register (protocol + flags). */
    virtual void setSocketMode(uint8_t socket, uint8_t mode) = 0;
    /** Set socket source port (host port). */
    virtual void setSocketSourcePort(uint8_t socket, uint16_t port) = 0;
    /** Set socket destination (remote peer) IP and port for CONNECT / UDP sendto. */
    virtual void setSocketDestination(uint8_t socket, const uint8_t* ip, uint16_t port) = 0;

    /** Get free TX buffer size for a socket. */
    virtual uint16_t getTXFreeSize(uint8_t socket) = 0;
    /** Get received RX buffer size for a socket. */
    virtual uint16_t getRXReceivedSize(uint8_t socket) = 0;

    /** Read a block of received data from socket RX buffer (does NOT advance pointer if peek!=0).
     */
    virtual void readSocketData(uint8_t socket, uint16_t srcPtr, uint8_t* dest, uint16_t len,
                                bool peek = false) = 0;
    /** Write a block of data into socket TX buffer at current write pointer (or offset if
     * provided). */
    virtual void writeSocketData(uint8_t socket, const uint8_t* data, uint16_t len) = 0;
    /** Write data with an explicit offset (for incremental UDP/TCP streaming assembly). */
    virtual void writeSocketDataOffset(uint8_t socket, uint16_t offset, const uint8_t* data,
                                       uint16_t len) = 0;

    /** Advance RX read pointer after data consumed. */
    virtual void advanceRX(uint8_t socket, uint16_t len) = 0;
    /** Advance TX write pointer after data prepared. */
    virtual void advanceTX(uint8_t socket, uint16_t len) = 0;
    /** Commit prepared TX buffer (trigger SEND command typically). */
    virtual void commitTX(uint8_t socket) = 0;

    // ---------------------------------------------------------------------
    // Socket pointer access (for RX_RD, TX_WR) to support portable buffering
    // ---------------------------------------------------------------------
    /** Get current RX read pointer for socket. */
    virtual uint16_t getSocketRXReadPointer(uint8_t socket) = 0;
    /** Set RX read pointer for socket. */
    virtual void setSocketRXReadPointer(uint8_t socket, uint16_t value) = 0;
    /** Get current TX write pointer for socket. */
    virtual uint16_t getSocketTXWritePointer(uint8_t socket) = 0;
    /** Set TX write pointer for socket. */
    virtual void setSocketTXWritePointer(uint8_t socket, uint16_t value) = 0;
};

#endif  // ETHERNET_CHIP_H