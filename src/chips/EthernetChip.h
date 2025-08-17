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
 * Provides common register interface for downstream systems.
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
     * Get maximum number of sockets supported by this chip
     * @return Maximum socket count
     */
    virtual uint8_t getMaxSockets() = 0;

    // ==== Common Register Interface ====

    /**
     * Read single byte from register
     * @param addr Register address
     * @return Register value
     */
    virtual uint8_t readRegister(uint16_t addr) = 0;

    /**
     * Write single byte to register
     * @param addr Register address  
     * @param data Data to write
     */
    virtual void writeRegister(uint16_t addr, uint8_t data) = 0;

    /**
     * Read multiple bytes from register
     * @param addr Register address
     * @param buf Buffer to store data
     * @param len Number of bytes to read
     * @return Number of bytes read
     */
    virtual uint16_t readRegister(uint16_t addr, uint8_t* buf, uint16_t len) = 0;

    /**
     * Write multiple bytes to register
     * @param addr Register address
     * @param buf Data buffer
     * @param len Number of bytes to write
     * @return Number of bytes written
     */
    virtual uint16_t writeRegister(uint16_t addr, const uint8_t* buf, uint16_t len) = 0;

    // ==== Socket-specific Register Interface ====

    /**
     * Read single byte from socket register
     * @param socket Socket number
     * @param addr Socket register address
     * @return Register value
     */
    virtual uint8_t readSocketRegister(uint8_t socket, uint16_t addr) = 0;

    /**
     * Write single byte to socket register
     * @param socket Socket number
     * @param addr Socket register address
     * @param data Data to write
     */
    virtual void writeSocketRegister(uint8_t socket, uint16_t addr, uint8_t data) = 0;

    /**
     * Read multiple bytes from socket register
     * @param socket Socket number
     * @param addr Socket register address
     * @param buf Buffer to store data
     * @param len Number of bytes to read
     * @return Number of bytes read
     */
    virtual uint16_t readSocketRegister(uint8_t socket, uint16_t addr, uint8_t* buf, uint16_t len) = 0;

    /**
     * Write multiple bytes to socket register
     * @param socket Socket number
     * @param addr Socket register address
     * @param buf Data buffer
     * @param len Number of bytes to write
     * @return Number of bytes written
     */
    virtual uint16_t writeSocketRegister(uint8_t socket, uint16_t addr, const uint8_t* buf, uint16_t len) = 0;

    // ==== Common Network Configuration Interface ====

    /**
     * Set MAC address
     * @param mac MAC address (6 bytes)
     */
    virtual void setMACAddress(const uint8_t* mac) = 0;

    /**
     * Get MAC address
     * @param mac Buffer for MAC address (6 bytes)
     */
    virtual void getMACAddress(uint8_t* mac) = 0;

    /**
     * Set IP address
     * @param ip IP address (4 bytes)
     */
    virtual void setIPAddress(const uint8_t* ip) = 0;

    /**
     * Get IP address
     * @param ip Buffer for IP address (4 bytes)
     */
    virtual void getIPAddress(uint8_t* ip) = 0;

    /**
     * Set gateway IP address
     * @param gateway Gateway IP address (4 bytes)
     */
    virtual void setGatewayIP(const uint8_t* gateway) = 0;

    /**
     * Get gateway IP address
     * @param gateway Buffer for gateway IP address (4 bytes)
     */
    virtual void getGatewayIP(uint8_t* gateway) = 0;

    /**
     * Set subnet mask
     * @param subnet Subnet mask (4 bytes)
     */
    virtual void setSubnetMask(const uint8_t* subnet) = 0;

    /**
     * Get subnet mask
     * @param subnet Buffer for subnet mask (4 bytes)
     */
    virtual void getSubnetMask(uint8_t* subnet) = 0;

    // ==== Socket Operations Interface ====

    /**
     * Execute socket command
     * @param socket Socket number
     * @param cmd Command code
     */
    virtual void executeSocketCommand(uint8_t socket, uint8_t cmd) = 0;

    /**
     * Get TX free size for socket
     * @param socket Socket number
     * @return Free size in bytes
     */
    virtual uint16_t getTXFreeSize(uint8_t socket) = 0;

    /**
     * Get RX received size for socket
     * @param socket Socket number
     * @return Received size in bytes
     */
    virtual uint16_t getRXReceivedSize(uint8_t socket) = 0;

    /**
     * Send data processing for socket
     * @param socket Socket number
     * @param data Data buffer
     * @param len Data length
     */
    virtual void sendDataProcessing(uint8_t socket, const uint8_t* data, uint16_t len) = 0;

    /**
     * Receive data processing for socket
     * @param socket Socket number
     * @param data Data buffer
     * @param len Data length
     * @param peek Whether to peek (don't advance read pointer)
     */
    virtual void receiveDataProcessing(uint8_t socket, uint8_t* data, uint16_t len, bool peek = false) = 0;

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
};

#endif  // ETHERNET_CHIP_H